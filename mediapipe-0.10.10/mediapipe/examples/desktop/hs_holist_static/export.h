#ifndef EXPORT_H
#define EXPORT_H

#ifdef MCP_EXPORTS
#define MCP_API __declspec(dllexport)
#else
#define MCP_API __declspec(dllimport)
#endif

#endif // !EXPORT_H

