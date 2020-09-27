# xml-parser
- this fucker is NOT perfect
- light weight xml parser using [pointer,size] into a source char*
- no memory allocation = doesn't copy pieces of the source (like tags,values) - it uses pointers
- can still have seg faults

# todo
- cdata
- comment
- ~~events while parsing (when parser encounters something)~~
- file travling = maybe a way to travel/seek file and remember positions in file (like w/ string)
- custom indexing
- optimization reading tags/attributes...
- there are 2 findById methods, there should be only 1
- support for FILE (only fstream for now)
- html5 support = this will suck, since it doesn't require "/" when closing certain tags
- better "bad-format" support
- profiling 
- logging/debug + external debug = bind external function to trigger on something
