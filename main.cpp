#include <iostream>
#include <fstream>
#include <functional>

#define USE_REFS 1
#define USE_EVENTS 1
#include "xml.hpp"


static char xmlstring[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                          "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n"
                          "  <asset>\n"
                          "    <created>2001-01-01T01:01:01Z</created>\n"
                          "    <modified>2001-01-01T01:01:01Z</modified>\n"
                          "  </asset>\n"
                          "  <library_images>\n"
                          "    <image id=\"Image\">\n"
                          "      <init_from>file:///Ambulance_Cabin_D.tga</init_from>\n"
                          "    </image>\n"
                          "    <image id=\"Image1\">\n"
                          "      <init_from>file:///Ambulance_Decals1_D.tga</init_from>\n"
                          "    </image>\n"
                          "    <image id=\"Image2\">\n"
                          "      <init_from>file:///Ambulance_Body_inside_D.tga</init_from>\n"
                          "    </image>\n"
                          "    <image id=\"Image3\">\n"
                          "      <init_from>file:///Ambulance_Glass_D.tga</init_from>\n"
                          "    </image>\n"
                          "    <image id=\"Image4\">\n"
                          "      <init_from>file:///Ambulance_Body_D.tga</init_from>\n"
                          "    </image>\n"
                          "    <image id=\"Image5\">\n"
                          "      <init_from>file:///Ambulance_Cabin_inside_D.tga</init_from>\n"
                          "      <extra>\n"
                          "      </extra>\n"
                          "    </image>\n"
                          "  </library_images>\n"
                          "  <library_materials>\n"
                          "    <material id=\"VisualMaterial\" name=\"Ambulance_Cabin__Ambulance_Cabin_D_tga\">\n"
                          "      <instance_effect url=\"#Effect\"/>\n"
                          "      <extra>\n"
                          "        <technique profile=\"FCOLLADA\">\n"
                          "          <user_properties>Ambulance_Cabin__Ambulance_Cabin_D_tga</user_properties>\n"
                          "        </technique>\n"
                          "      </extra>\n"
                          "    </material>\n"
                          "  </library_materials>\n"
                          "</COLLADA>\n";

using namespace sys::xml;

int main() {
  
  tree_t* tree = nullptr;
  
  try {
  
    std::fstream fs("../test.xml", std::ios::in|std::ios::binary);
    
    if (!fs) {
      throw std::string("failed to open");
    }
    
    sys::xml::parser_t parser;
    
    parser.onAttribute("profile", "FCOLLADA",[](sys::xml::attribute_t*, sys::xml::element_t* e) { std::cout << "element:" << e->name << std::endl; });
    
    tree = parser.read(fs);
    fs.close();
    
    std::cout << *tree << std::endl;

    
    auto exts = tree->findByName("extra");
    
        
    auto res = tree->findById("Image4");
    
    if (res) {
      std::cout << *res << std::endl;
    }
    
    
  } catch (std::string& e) {
    
    std::cout << "error:" << e << std::endl;
  }
  
  delete tree;
  
  return 0;
}
