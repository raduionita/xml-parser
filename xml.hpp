#ifndef XMLPARSER_XML_HPP
#define XMLPARSER_XML_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <cstring>

// #define USE_REF 1

namespace sys::xml {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  struct tree_t;
  struct element_t;
  
  bool strncmp(const char* lhs, const char* rhs, uint32_t n);
  
  tree_t* parse(std::fstream&, uint32_t);
  tree_t* parse(const char*, uint32_t);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  struct view_t { 
    char*    from {nullptr}; 
    uint32_t size {0}; 
    
    friend std::ostream& operator <<(std::ostream& out, const view_t& view);
  };
  
  struct node_t { 
    node_t() = default;
    virtual ~node_t() = default;
  };
  
  struct attribute_t { 
    view_t    name; 
    view_t     value;
#ifdef USE_REF
    element_t* ref {nullptr};
#endif // USE_REF
  };
  
  struct cdata_t : node_t { };
  
  struct comment_t : node_t { view_t value; };
  
  struct text_t : node_t { view_t value; };
  
  struct element_t : node_t { 
  // fields
    view_t                   name; 
    view_t                   id; 
    std::vector<attribute_t> attributes; 
    std::vector<element_t*>  children; 
    element_t*               parent {nullptr}; // nullptr = only root 
    text_t                   text;
  // ctor/dtor
    ~element_t() { for (auto it = children.begin(); it != children.end(); ++it) { delete *it; *it = nullptr; } }
  // friends  
    friend std::ostream& operator <<(std::ostream& out, const element_t& tree);
  // methods
    std::vector<element_t*> findByName(const char tName[]) {
      std::vector<element_t*> all;
      
      if (strncmp(name.from, tName, strlen(tName)))
        all.push_back(this);
      
      for (auto it = children.begin(); it != children.end(); ++it) {
        std::vector<element_t*> out = (*it)->findByName(tName);
        all.insert(all.begin(),out.begin(),out.end());
      }
      
      return all;
    }
  };
  
  struct tree_t {
  // fields
    enum : uint32_t { OWNER = 1, };
    uint32_t                     flags;
    char*                        source {nullptr};
    view_t                       type;
    element_t*                   root {nullptr};
    std::vector<attribute_t>     attributes;
    std::map<view_t*,element_t*> index;  // @todo: replace w/ indices a map of indices, where u index stuff by something, id/tag...
  // ctor/dtor
    ~tree_t() { if (root) { delete root; root = nullptr; } if ((flags & OWNER) && (source)) { delete [] source; source = nullptr; } }
  // friends
    friend std::ostream& operator <<(std::ostream& out, const tree_t& tree);
  // methods
    element_t* findById(const char id[]) {
      short i {0};
      // in case id starts w/ # // id="#some"
      if (id[0] == '#')
        i++;
      for (auto it = index.begin(); it != index.end(); ++it)
        if (strncmp(it->first->from, id+i, strlen(id)-i))
          return it->second;
      return nullptr;
    }
    element_t* findById(const view_t& id) {
      short i {0};
      // in case id starts w/ # // id="#some"
      if (id.from[0] == '#')
        i++;
      for (auto it = index.begin(); it != index.end(); ++it)
        if (strncmp(it->first->from, id.from+i, id.size-i))
          return it->second;
      return nullptr;
    }
    std::vector<element_t*> findByName(const char tName[]) {
      return root->findByName(tName);
    }
  };
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  std::ostream& operator <<(std::ostream& out, const tree_t& tree) {
    out << tree.type << std::endl;
    for (auto it = tree.attributes.begin(); it != tree.attributes.end(); ++it) {
      out << ".attributes[" << it->name << "]=" << it->value << std::endl;
    }
    if (tree.root)  {
      out << *tree.root;
    }
    return out;
  }
  
  std::ostream& operator <<(std::ostream& out, const element_t& element) {
    out << element.name << std::endl;
    for (auto it = element.attributes.begin(); it != element.attributes.end(); ++it) {
      out << ".attributes[" <<  it->name << "]=" << it->value << std::endl;
    }
    
    out << ".text=" << element.text.value << std::endl; 
    
    out << ".children=" << std::endl;
    for (auto& child : element.children) {
      out << *child;
    }
    return out;
  }
  
  std::ostream& operator <<(std::ostream& out, const view_t& view) {
    return out.write(view.from,view.size);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  bool strncmp(const char* lhs, const char* rhs, uint32_t n) { for (uint32_t i = 0; i < n; i++) if (lhs[i] != rhs[i]) return false; return true; }
  
  uint32_t alnumspn(const char* src) {
    uint32_t i {0};
    while(!isalnum(src[i]))
      i++;
    return i;
  }  
  
  uint32_t charspn(const char* src, char c) {
    uint32_t i {0};
    while(src[i] != c)
      i++;
    return i;
  }
  
  uint32_t taglen(const char* src) {
    uint32_t i {0};
    while (isalnum(src[i]) || src[i] == '_')
      i++;
    return i;
  }
  
  uint32_t attrlen(const char* src) {
    uint32_t i {0};
    // @todo: len of an attribute name
    return i;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  tree_t* parse(std::fstream& fs, uint32_t flags = 0) {
    fs.seekg(0, fs.end);
    size_t len = fs.tellg();
    fs.seekg(0, fs.beg);
    
    char* src = new char[len+1];
    
    fs.read(src,len);
    
    src[len] = '\0';
    
    return parse(src, flags | tree_t::OWNER);
  }
  
  tree_t* parse(const char* source, uint32_t flags = 0) {
    tree_t*    tTree    = new tree_t;
    char* tPointer = tTree->source = const_cast<char*>(source);
    
    element_t* tCurrent = nullptr;
    element_t* tParent  = nullptr;
    
#ifdef USE_REF
    std::vector<attribute_t*> tRefs; // id=>element
#endif // USE_REF
    
    while (*tPointer) { // if not EOF
      if (tPointer[0] == '\n' || tPointer[0] == '\r' || tPointer[0] == '\t' || tPointer[0] == ' ') {
        tPointer += 1;
        
        continue;
      } else if (tPointer[0] == '<' && tPointer[1] == '?') { // xml:tag
        tPointer += 1;
        
        // span to next > // not including
        uint32_t nLength = charspn(tPointer, '>');   // value
        // span to next alpha-numeric
        uint32_t i = alnumspn(tPointer/*until alphanum*/);
        uint32_t j {0};
        
        // tag // from when alpha num starts
        tTree->type.from = tPointer + i;
        // tag // to where tag ends
        tTree->type.size = taglen(tPointer + i);
        
        i += tTree->type.size;
        
        // attributes
        while((i + (j = alnumspn(tPointer + i/*until alphanum*/))) < nLength) {
          i += j;
          attribute_t tAttribute;
          
          tAttribute.name = {tPointer + i, charspn(tPointer + i, '=')};
          
          i += tAttribute.name.size; 
          i += 2; // ="
          
          tAttribute.value = {tPointer + i, charspn(tPointer + i, '"')};
          
          tTree->attributes.push_back(tAttribute);
          
          // pass end of value
          i += tAttribute.value.size; 
          i += 1; // "
        }
        
        tPointer += i;
        tPointer += 2; // ? + >
        
        continue;
      } else if (tPointer[0] == '<' && tPointer[1] != '/') { // tag:start
        tPointer += 1;
        
        // parent = previous current
        tParent  = tCurrent;
        // new current
        tCurrent = new element_t;
        tCurrent->parent = tParent;
        
        // span to next > // not including
        uint32_t nLength = charspn(tPointer, '>');   // value
        // span to next alpha-numeric
        uint32_t i = alnumspn(tPointer/*until alphanum*/);;
        uint32_t j {0};
        
        // tag // from when alpha num starts
        tCurrent->name.from = tPointer + i;
        // tag // to where tag ends
        tCurrent->name.size = taglen(tPointer + i);
      
        i += tCurrent->name.size;
        
        // if i cannot pass the length of the tag
        while((i + (j = alnumspn(tPointer + i/*until alphanum*/))) < nLength) { // attributes
          i += j;
          attribute_t tAttribute;
          
          // attribute // from when alpha num starts -> until =
          tAttribute.name = {tPointer + i, charspn(tPointer + i, '=')};
          
          i += tAttribute.name.size; 
          i += 2; // ="
          
          // attribute.value // from where we at, until "
          tAttribute.value = {tPointer + i, charspn(tPointer + i, '"')};
          
          tCurrent->attributes.push_back(tAttribute);
          
#ifdef USE_REF
          // find ref
          if (tAttribute.value.from[0] == '#') {
            // like source="#someid"
            tAttribute.ref = tTree->findById(tAttribute.value);
            if (tAttribute.ref == nullptr) {
              tRefs.push_back(&tCurrent->attributes.back());
            }
          }
#endif // USE_REF
          
          // remeber element id=
          if (strncmp(tAttribute.name.from, "id", 2)) {
            tCurrent->id = tCurrent->attributes.back().value;
            
            tTree->index[&tCurrent->id] = tCurrent;
            
#ifdef USE_REF
            // fill refs
            for (auto it = tRefs.begin(); it != tRefs.end(); ++it) {
              if (strncmp((*it)->value.from+1, tAttribute.value.from, tAttribute.value.size)) {
                (*it)->ref = tCurrent;
              }
            }
#endif // USE_REF
          }
          
          // pass end of value
          i += tAttribute.value.size;
          // pass "
          i += 1; 
        }
        
        // set up root
        if (tTree->root == nullptr) {
          tTree->root = tCurrent;
        }
        
        // add child to parent (like current element to root)
        if (tParent) {
          tParent->children.push_back(tCurrent);
        }
        
        // advance
        tPointer += i;
        
        // tag closes itself 
        if (tPointer[0] == '/') {
          tPointer += 1;
          tCurrent = tCurrent->parent;
        }
        
        // pass > 
        tPointer += 1;
        
        continue;
      } else if (tPointer[0] == '<' && tPointer[1] == '/') { // tag:end
        // pass </
        tPointer += 2;
        
        // span to next > //  not including
        uint32_t nLength = charspn(tPointer, '>');   
        uint32_t i {0};
        
        // if current is null => bad close - close more then u should
        if (tCurrent == nullptr) {
          throw std::string(tPointer, nLength).append(" bad close!");
        }
        
        if (!strncmp(tPointer, tCurrent->name.from, tCurrent->name.size)) {
          throw std::string(tPointer-2, nLength+3).append(" close mismatch!");
        }
        
        // closing by reverting current to the previous parent
        tCurrent = tCurrent->parent;
        
        // unnecessary, but might be usefull later
        i += nLength;
        
        // advance
        tPointer += i;
        
        // pass >
        tPointer += 1;
        
        continue;
      } else if (tPointer[0] != '<') {
        
        uint32_t nLength = charspn(tPointer, '<');   // value
        
        tCurrent->text.value.from = tPointer;
        tCurrent->text.value.size = nLength;
        
        tPointer += nLength;
        
        continue;
      }
      
      tPointer++;
    }
// parsed
    return tTree;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //XMLPARSER_XML_HPP
