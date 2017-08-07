import qbs

Project {
    minimumQbsVersion: "1.6.0"
    CppApplication {
        type: ["application"]
        consoleApplication: true
        files: ["*.cpp","*.hpp"]
        cpp.defines: ['BOOST_HANA_CONFIG_ENABLE_STRING_UDL']
        cpp.cxxLanguageVersion: "c++1z"
        Depends { name: "Basic Reflection" }
        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }
} 
 
