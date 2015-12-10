#include <phpcpp.h>
#include <iostream>
#include <sys/stat.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace soula {
    template<typename... Args>
    std::string string_format(const char* fmt, Args... args)
    {
        size_t size = snprintf(nullptr, 0, fmt, args...);
        std::string buf;
        buf.reserve(size + 1);
        buf.resize(size);
        snprintf(&buf[0], size + 1, fmt, args...);
        return buf;
    }

    Php::Value tesseract_recognize(Php::Parameters &params)
    {
        char *outText;
        const char * filename = params[0];
        const char *language    = Php::ini_get("php-tesseract.language");
        const char *tessdataDir = Php::ini_get("php-tesseract.tessdata-dir");

        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

        struct stat buffer;
        if(stat (tessdataDir, &buffer) != 0) {
            Php::error << soula::string_format("%s tessdata dir not exists.", tessdataDir) << std::flush;
        }

        if(stat (filename, &buffer) != 0) {
            Php::error << soula::string_format("%s file not exists.", filename) << std::flush;
        }
    
        if (api->Init(tessdataDir, language)) {
            Php::error << "could not initialize tesseract." << std::flush;
            return false;
        }

        // Open input image with leptonica library
        Pix *image = pixRead(filename);
        api->SetImage(image);
    
        // Get OCR result
        outText = api->GetUTF8Text();

        // Destroy used object and release memory
        api->End();

        //delete [] outText;
        pixDestroy(&image);

        //#Php::out << "example output" << std::endl;
        return outText;
    }
}




/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("php-tesseract", "1.0");

        extension.add("tesseract_recognize", soula::tesseract_recognize, { Php::ByVal("filename", Php::Type::String, true) });

        extension.add(Php::Ini("php-tesseract.language", "eng"));
        extension.add(Php::Ini("php-tesseract.tessdata-dir", "/usr/local/tesseract/tessdata"));
        // @todo    add your own functions, classes, namespaces to the extension
        
        // return the extension
        return extension;
    }
}
