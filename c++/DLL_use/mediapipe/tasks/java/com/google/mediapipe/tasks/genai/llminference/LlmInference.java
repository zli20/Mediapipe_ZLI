package com.google.mediapipe.tasks.genai.llminference;

import android.content.Context;
import com.google.auto.value.AutoValue;
import com.google.mediapipe.tasks.core.ErrorListener;
import com.google.mediapipe.tasks.core.LlmTaskRunner;
import com.google.mediapipe.tasks.core.OutputHandler.ProgressListener;
import com.google.mediapipe.tasks.core.TaskOptions;
import com.google.mediapipe.tasks.core.jni.proto.LlmOptionsProto.LlmSessionConfig;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.atomic.AtomicBoolean;

/** LlmInference Task Java API */
public class LlmInference implements AutoCloseable {
  private static final char TOKEN_SPLITTER = '▁'; // Note this is NOT an underscore: ▁(U+2581)
  private static final String NEW_LINE = "<0x0A>";
  private static final String EOD = "\\[eod\\]";

  private static final int NUM_DECODE_STEPS_PER_SYNC = 3;

  private final LlmTaskRunner taskRunner;
  private final AtomicBoolean isProcessing;

  static {
    System.loadLibrary("llm_inference_engine_jni");
  }

  /** Creates an LlmInference Task. */
  public static LlmInference createFromOptions(Context context, LlmInferenceOptions options) {
    // Configure LLM session config.
    LlmSessionConfig.Builder sessionConfig = LlmSessionConfig.newBuilder();

    sessionConfig.setModelPath(options.modelPath());
    sessionConfig.setCacheDir(context.getCacheDir().getAbsolutePath());
    sessionConfig.setNumDecodeStepsPerSync(NUM_DECODE_STEPS_PER_SYNC);
    sessionConfig.setMaxSequenceLength(options.maxSequenceLength());
    sessionConfig.setTopk(options.topK());
    sessionConfig.setTemperature(options.temperature());
    sessionConfig.setRandomSeed(options.randomSeed());

    return new LlmInference(sessionConfig.build(), options.resultListener());
  }

  /** Constructor to initialize an {@link LlmInference}. */
  private LlmInference(
      LlmSessionConfig sessionConfig, Optional<ProgressListener<String>> resultListener) {

    Optional<ProgressListener<List<String>>> llmResultListener;
    if (resultListener.isPresent()) {
      llmResultListener =
          Optional.of(
              new ProgressListener<List<String>>() {
                private boolean receivedFirstToken = false;

                @Override
                public void run(List<String> partialResult, boolean done) {
                  String result =
                      decodeResponse(
                          partialResult, /* stripLeadingWhitespace= */ !receivedFirstToken);
                  if (done) {
                    receivedFirstToken = false; // Reset to initial state
                    isProcessing.set(false);
                    resultListener.get().run(result, done);
                  } else if (!result.isEmpty()) {
                    receivedFirstToken = true;
                    resultListener.get().run(result, done);
                  }
                }
              });
    } else {
      llmResultListener = Optional.empty();
    }

    this.taskRunner = new LlmTaskRunner(sessionConfig, llmResultListener);
    this.isProcessing = new AtomicBoolean(false);
  }

  /**
   * Generates a response based on the input text.
   *
   * @param inputText a {@link String} for processing.
   */
  public String generateResponse(String inputText) {
    validateState();
    isProcessing.set(true);
    List<String> tokens = taskRunner.predictSync(inputText);
    isProcessing.set(false);
    return decodeResponse(tokens, /* stripLeadingWhitespace= */ true);
  }

  /**
   * Generates a response based on the input text.
   *
   * @param inputText a {@link String} for processing.
   */
  public void generateResponseAsync(String inputText) {
    validateState();
    isProcessing.set(true);
    taskRunner.predictAsync(inputText);
  }

  /** Decodes the response from the LLM engine and returns a human-readable string. */
  private static String decodeResponse(List<String> responses, boolean stripLeadingWhitespace) {
    if (responses.isEmpty()) {
      // Technically, this is an error. We should always get at least one response.
      return "";
    }

    String response = responses.get(0); // We only use the first response
    response = response.replace(TOKEN_SPLITTER, ' '); // Note this is NOT an underscore: ▁(U+2581)
    response = response.replace(NEW_LINE, "\n"); // Replace <0x0A> token with newline

    if (stripLeadingWhitespace) {
      response = stripLeading(response); // Strip all leading spaces for the first output
    }

    return response.split(EOD, -1)[0];
  }

  private void validateState() {
    if (isProcessing.get()) {
      throw new IllegalStateException("Previous invocation still processing. Wait for done=true.");
    }
  }

  /** Closes and cleans up the {@link LlmInference}. */
  @Override
  public void close() {
    taskRunner.close();
  }

  /** Options for setting up an {@link LlmInference}. */
  @AutoValue
  public abstract static class LlmInferenceOptions extends TaskOptions {
    /** Builder for {@link LlmInferenceOptions}. */
    @AutoValue.Builder
    public abstract static class Builder {

      /** Sets the model path for the text generator task. */
      public abstract Builder setModelPath(String modelPath);

      /** Sets the result listener to invoke with the async API. */
      public abstract Builder setResultListener(ProgressListener<String> listener);

      /** Sets the error listener to invoke with the async API. */
      public abstract Builder setErrorListener(ErrorListener listener);

      /**
       * Configures the sequence length stands (i.e. the total number of tokens from input and
       * output).
       */
      public abstract Builder setMaxSequenceLength(int maxSequenceLength);

      /**
       * Configures the top K number of tokens to be sampled from for each decoding step. A value of
       * 1 means greedy decoding. The default value is 40.
       */
      public abstract Builder setTopK(int topK);

      /**
       * Configures randomness when decoding the next token. A value of 0.0f means greedy decoding.
       * The default value is 0.8f.
       */
      public abstract Builder setTemperature(float temperature);

      /** Configures random seed for sampling tokens. */
      public abstract Builder setRandomSeed(int randomSeed);

      abstract LlmInferenceOptions autoBuild();

      /** Validates and builds the {@link ImageGeneratorOptions} instance. */
      public final LlmInferenceOptions build() {
        return autoBuild();
      }
    }

    /** The path that points to the tflite model file. */
    public abstract String modelPath();

    /**
     * The total length of the kv-cache. In other words, this is the total number of input + output
     * tokens the model needs to handle.
     */
    public abstract int maxSequenceLength();

    /**
     * Top K number of tokens to be sampled from for each decoding step. A value of 1 means greedy
     * decoding.
     */
    public abstract int topK();

    /** Randomness when decoding the next token. A value of 0.0f means greedy decoding. */
    public abstract float temperature();

    /** Random seed for sampling tokens. */
    public abstract int randomSeed();

    /** The result listener to use for the {@link LlmInference#generateAsync} API. */
    abstract Optional<ProgressListener<String>> resultListener();

    /** The error listener to use for the {@link LlmInference#generateAsync} API. */
    abstract Optional<ErrorListener> errorListener();

    /** Instantiates a new LlmInferenceOptions builder. */
    public static Builder builder() {
      return new AutoValue_LlmInference_LlmInferenceOptions.Builder()
          .setMaxSequenceLength(512)
          .setTopK(40)
          .setTemperature(0.8f)
          .setRandomSeed(0);
    }
  }

  static String stripLeading(String text) {
    // stripLeading() implementation for Android < 33
    int left = 0;
    while (left < text.length()) {
      final int codepoint = text.codePointAt(left);
      if (!Character.isWhitespace(codepoint)) {
        break;
      }
      left += Character.charCount(codepoint);
    }
    return text.substring(left);
  }
}
