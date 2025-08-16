#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <cstring>
#include <fstream>
#include <format>
#include <exception>
#include <utility>
#include <sstream>
#include <ranges>

using namespace std;
namespace fs = std::filesystem;


auto file2str(string path) -> string;
auto byte2hex(unsigned char d) -> const char*;
auto file2code(string filename) -> string;

constexpr auto Pack = "Res";

int main(int argc, char* argv[]){
    if(argc < 2){
        throw runtime_error("Usage: file2cpp  resDir  outDir");
    }

    auto header_name = format("{}.hpp", Pack);
    auto cpp_name    = format("{}.cpp", Pack);

    auto res_dir = fs::path(argv[1]);
    auto out_dir = fs::path(argv[2]);

    static auto Pack_Header_File = fstream(fs::path(out_dir) / header_name, ios::out | ios::trunc);
    static auto Pack_CPP_File = fstream(fs::path(out_dir) / cpp_name, ios::out | ios::trunc);

    Pack_Header_File << "#include <string>" << endl;
    Pack_Header_File << "#include <unordered_map>" << endl;
    Pack_Header_File << "#include <utility>" << endl;
    Pack_Header_File << endl;

    for (auto e : fs::recursive_directory_iterator(res_dir) | views::filter([](const auto& e) { return e.is_regular_file(); }))
    {
        auto filename = e.path().generic_string();
        Pack_CPP_File << file2code(filename) << endl;
        Pack_Header_File << format("extern unsigned char res_{}[];", hash<string>{}(filename)) << endl;
    }

    // -----

    Pack_Header_File << "std::unordered_map<std::string, std::pair<const unsigned char*, size_t>> Resources {" << endl;

    for (auto e : fs::recursive_directory_iterator(res_dir)
    | views::filter([](const auto& e) { return e.is_regular_file(); }))
    {
        auto filename = e.path().generic_string();
        auto size = e.file_size();

        Pack_Header_File << '\t' << format(R"({{ "{}", {{ res_{}, {} }} }},)", filename, hash<string>{}(filename), size) << endl;
    }

    Pack_Header_File << "};" << endl;
}

inline auto file2str(string path) -> string
{
    ifstream file(path, ios::binary);

    if (!file){
        throw runtime_error(format("Couldnt open file [{}] : {}", path, strerror(errno)) );
    }else{
        auto it = istreambuf_iterator<char>(file);
        auto end = istreambuf_iterator<char>();
        return { it, end };
    }
}

auto byte2hex(unsigned char d) -> const char*
{
    constexpr char hex_digits[] = "0123456789ABCDEF";
    static char result[] { '0', 'x', 'n', 'n', '\0'};

    result[2] = hex_digits[(d >> 4) & 0xF];
    result[3] = hex_digits[d & 0xF];

    return result;
}

auto file2code(string filename) -> string
{
    auto buff = stringstream{};
    auto content = file2str(filename);

    buff << format("constexpr static unsigned char res_{}[] {{", hash<string>{}(filename));
    
    for (const auto& b : content) buff << byte2hex(b) << ',';

    buff << "};";
    return buff.str();
}
