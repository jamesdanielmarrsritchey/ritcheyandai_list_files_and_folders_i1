#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <getopt.h>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iterator>

bool listFilesRecursively(const std::filesystem::path& path, const std::string& delimiter, bool exclude_dirs, bool exclude_files) {
    if (!std::filesystem::exists(path)) {
        std::cout << "Path does not exist: " << path << std::endl;
        return false;
    }

    std::vector<std::string> paths;

    for (auto& p: std::filesystem::recursive_directory_iterator(path)) {
        if ((exclude_dirs && p.is_directory()) || (exclude_files && !p.is_directory())) {
            continue;
        }
        std::string pathStr = p.path().string();
        pathStr.erase(std::remove(pathStr.begin(), pathStr.end(), '\"' ), pathStr.end());
        paths.push_back(pathStr);
    }

    std::ostringstream oss;
    if (!paths.empty()) {
        std::copy(paths.begin(), paths.end()-1, std::ostream_iterator<std::string>(oss, delimiter.c_str()));
        oss << *paths.rbegin();
    }
    std::cout << oss.str();

    return !paths.empty();
}

int main(int argc, char* argv[]) {
    std::filesystem::path path;
    std::string delimiter = "\n";
    bool exclude_dirs = false;
    bool exclude_files = false;
    int c;

    while (true) {
        static struct option long_options[] = {
            {"source_dir", required_argument, 0, 's'},
            {"delimiter", required_argument, 0, 'd'},
            {"exclude_dirs", required_argument, 0, 'e'},
            {"exclude_files", required_argument, 0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "s:d:e:f:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 's':
                path = optarg;
                break;

            case 'd':
                delimiter = optarg;
                if (delimiter == "\\n") {
                    delimiter = "\n";
                }
                else if (delimiter == "\\t") {
                    delimiter = "\t";
                }
                break;

            case 'e':
                exclude_dirs = std::string(optarg) == "true";
                break;

            case 'f':
                exclude_files = std::string(optarg) == "true";
                break;

            case '?':
                break;

            default:
                abort();
        }
    }

    if (path.empty()) {
        std::cout << "Please provide a directory path with --source_dir option." << std::endl;
        return 1;
    }

    bool result = listFilesRecursively(path, delimiter, exclude_dirs, exclude_files);

    if (!result) {
        return 0;
    }

    return 0;
}