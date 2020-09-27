# usage:file
```c++
#define USE_REFS
#define USE_EVENTS
#import "xml.hpp"

std::fstream fs("../test.xml", std::ios::in|std::ios::binary);

sys::xml::parser_t parser;
parser.onAttribute("profile", "FCOLLADA",[](sys::xml::attribute_t*, sys::xml::element_t* e) { std::cout << "element:" << e->name << std::endl; });

sys::xml::tree_t* tree = parser.read(fs); // @throws "something"

fs.close();

std::cout << *tree << std::endl;

delete tree; // MUST
```

# usage:string
```c++
const char src[] = "<xml string>";

sys::xml::parser_t parser;
parser.onElement("tag_name", [](sys::xml::element_t* e) { std::cout << "element:" << e->name << std::endl; });

sys::xml::tree_t* tree = parser.read(src); // @throws "something"

delete tree; // MUST
```

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
- smart-pointer-like wrapper around tree_t* (returned from parser::read)
- clean up dependencies inside "xml.hpp"
