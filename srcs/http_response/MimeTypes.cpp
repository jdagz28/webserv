/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 22:18:43 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/13 04:46:29 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <map>


std::map<std::string, std::string>& MimeTypes()
{
    static std::map<std::string, std::string> mimeTypes;
    
    if (mimeTypes.empty())
    {
        // Text types
        mimeTypes["html"] = "text/html";
        mimeTypes["htm"] = "text/html";
        mimeTypes["shtml"] = "text/html";
        mimeTypes["css"] = "text/css";
        mimeTypes["xml"] = "text/xml";
        mimeTypes["mml"] = "text/mathml";
        mimeTypes["txt"] = "text/plain";
        mimeTypes["jad"] = "text/vnd.sun.j2me.app-descriptor";
        mimeTypes["wml"] = "text/vnd.wap.wml";
        mimeTypes["htc"] = "text/x-component";

        // Image types
        mimeTypes["gif"] = "image/gif";
        mimeTypes["jpeg"] = "image/jpeg";
        mimeTypes["jpg"] = "image/jpeg";
        mimeTypes["avif"] = "image/avif";
        mimeTypes["png"] = "image/png";
        mimeTypes["svg"] = "image/svg+xml";
        mimeTypes["svgz"] = "image/svg+xml";
        mimeTypes["tif"] = "image/tiff";
        mimeTypes["tiff"] = "image/tiff";
        mimeTypes["wbmp"] = "image/vnd.wap.wbmp";
        mimeTypes["webp"] = "image/webp";
        mimeTypes["ico"] = "image/x-icon";
        mimeTypes["jng"] = "image/x-jng";
        mimeTypes["bmp"] = "image/x-ms-bmp";

        // Font types
        mimeTypes["woff"] = "font/woff";
        mimeTypes["woff2"] = "font/woff2";

        // Application types
        mimeTypes["js"] = "application/javascript";
        mimeTypes["atom"] = "application/atom+xml";
        mimeTypes["rss"] = "application/rss+xml";
        mimeTypes["jar"] = "application/java-archive";
        mimeTypes["war"] = "application/java-archive";
        mimeTypes["ear"] = "application/java-archive";
        mimeTypes["json"] = "application/json";
        mimeTypes["hqx"] = "application/mac-binhex40";
        mimeTypes["doc"] = "application/msword";
        mimeTypes["pdf"] = "application/pdf";
        mimeTypes["ps"] = "application/postscript";
        mimeTypes["eps"] = "application/postscript";
        mimeTypes["ai"] = "application/postscript";
        mimeTypes["rtf"] = "application/rtf";
        mimeTypes["m3u8"] = "application/vnd.apple.mpegurl";
        mimeTypes["kml"] = "application/vnd.google-earth.kml+xml";
        mimeTypes["kmz"] = "application/vnd.google-earth.kmz";
        mimeTypes["xls"] = "application/vnd.ms-excel";
        mimeTypes["eot"] = "application/vnd.ms-fontobject";
        mimeTypes["ppt"] = "application/vnd.ms-powerpoint";
        mimeTypes["odg"] = "application/vnd.oasis.opendocument.graphics";
        mimeTypes["odp"] = "application/vnd.oasis.opendocument.presentation";
        mimeTypes["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
        mimeTypes["odt"] = "application/vnd.oasis.opendocument.text";
        mimeTypes["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        mimeTypes["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        mimeTypes["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        mimeTypes["wmlc"] = "application/vnd.wap.wmlc";
        mimeTypes["wasm"] = "application/wasm";
        mimeTypes["7z"] = "application/x-7z-compressed";
        mimeTypes["cco"] = "application/x-cocoa";
        mimeTypes["jardiff"] = "application/x-java-archive-diff";
        mimeTypes["jnlp"] = "application/x-java-jnlp-file";
        mimeTypes["run"] = "application/x-makeself";
        mimeTypes["pl"] = "application/x-perl";
        mimeTypes["pm"] = "application/x-perl";
        mimeTypes["prc"] = "application/x-pilot";
        mimeTypes["pdb"] = "application/x-pilot";
        mimeTypes["rar"] = "application/x-rar-compressed";
        mimeTypes["rpm"] = "application/x-redhat-package-manager";
        mimeTypes["sea"] = "application/x-sea";
        mimeTypes["swf"] = "application/x-shockwave-flash";
        mimeTypes["sit"] = "application/x-stuffit";
        mimeTypes["tcl"] = "application/x-tcl";
        mimeTypes["tk"] = "application/x-tcl";
        mimeTypes["der"] = "application/x-x509-ca-cert";
        mimeTypes["pem"] = "application/x-x509-ca-cert";
        mimeTypes["crt"] = "application/x-x509-ca-cert";
        mimeTypes["xpi"] = "application/x-xpinstall";
        mimeTypes["xhtml"] = "application/xhtml+xml";
        mimeTypes["xspf"] = "application/xspf+xml";
        mimeTypes["zip"] = "application/zip";

        // Application/octet-stream
        mimeTypes["bin"] = "application/octet-stream";
        mimeTypes["exe"] = "application/octet-stream";
        mimeTypes["dll"] = "application/octet-stream";
        mimeTypes["deb"] = "application/octet-stream";
        mimeTypes["dmg"] = "application/octet-stream";
        mimeTypes["iso"] = "application/octet-stream";
        mimeTypes["img"] = "application/octet-stream";
        mimeTypes["msi"] = "application/octet-stream";
        mimeTypes["msp"] = "application/octet-stream";
        mimeTypes["msm"] = "application/octet-stream";

        // Audio types
        mimeTypes["mid"] = "audio/midi";
        mimeTypes["midi"] = "audio/midi";
        mimeTypes["kar"] = "audio/midi";
        mimeTypes["mp3"] = "audio/mpeg";
        mimeTypes["ogg"] = "audio/ogg";
        mimeTypes["m4a"] = "audio/x-m4a";
        mimeTypes["ra"] = "audio/x-realaudio";

        // Video types
        mimeTypes["3gpp"] = "video/3gpp";
        mimeTypes["3gp"] = "video/3gpp";
        mimeTypes["ts"] = "video/mp2t";
        mimeTypes["mp4"] = "video/mp4";
        mimeTypes["mpeg"] = "video/mpeg";
        mimeTypes["mpg"] = "video/mpeg";
        mimeTypes["mov"] = "video/quicktime";
        mimeTypes["webm"] = "video/webm";
        mimeTypes["flv"] = "video/x-flv";
        mimeTypes["m4v"] = "video/x-m4v";
        mimeTypes["mng"] = "video/x-mng";
        mimeTypes["asx"] = "video/x-ms-asf";
        mimeTypes["asf"] = "video/x-ms-asf";
        mimeTypes["wmv"] = "video/x-ms-wmv";
        mimeTypes["avi"] = "video/x-msvideo";
    }
    
    return (mimeTypes);
}

std::string getMimeType(const std::string &extension)
{
    const std::map<std::string, std::string>& mime_types = MimeTypes();

    std::map<std::string, std::string>::const_iterator it = mime_types.find(extension);
    if (it != mime_types.end()) {
        return (it->second);
    }
    return (std::string());  
}