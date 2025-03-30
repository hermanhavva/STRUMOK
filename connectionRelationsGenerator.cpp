#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdlib>

void strumok(int T = 11) {
    const std::string cipher_name = "strumok";
    const int recommended_mg = 7;
    const int recommended_ms = 9;

    std::ostringstream oss;
    oss << "connection relations\n";

    for (int t = 0; t < T; ++t) {
        oss << "S_" << (t + 16) << ", S_" << (t + 13) << ", S_" << (t + 11) << ", S_" << t << "\n";
        oss << "Z_" << t << ", S_" << (t + 15) << ", R_" << (t + 1) << ", R_" << t << ", S_" << t << "\n";
        oss << "R_" << (t + 2) << ", S_" << (t + 13) << ", R_" << t << "\n";
    }
    oss << "known\n";
    for (int t = 0; t < T; ++t) {
        oss << "Z_" << t << "\n";
    }
    oss << "end";

    std::filesystem::path output_dir = std::filesystem::current_path();
    std::filesystem::path ossfile_path = output_dir / (cipher_name + "_" + std::to_string(T) + "clk_mg" +
                                                       std::to_string(recommended_mg) + "_ms" +
                                                       std::to_string(recommended_ms) + "_wl" +
                                                        + "11.txt");

    std::ofstream relation_file(ossfile_path);
    if (relation_file.is_open()) {
        relation_file << oss.str();
        relation_file.close();
    } else {
        std::cerr << "Failed to open file: " << ossfile_path << std::endl;
    }

    std::string command = "docker cp " + ossfile_path.string() + " bea00866ba82:/home/tools/autoguess";
    int result = system(command.c_str());
    
    if (result == 0) {
        printf("File copied successfully!\n");
    } else {
        printf("Error copying file.\n");
    }
}

int main() {
    strumok();
    return 0;
}