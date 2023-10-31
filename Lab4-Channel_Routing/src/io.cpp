#include "io.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>
#include <ctime>
#include <string.h>
std::stringstream ss;
void tokenLine(std::vector<std::string> &tokens, std::string line)
{
    ss.clear(); ss.str(line);
    tokens.clear(); tokens.resize(0);
    std::string intermediate;
    while(true){
        ss >> intermediate;
        if(ss.fail()) break;
        tokens.push_back(intermediate);
    }
}

void io::readChannel(char const *file_path)
{
    std::ifstream in_file(file_path);
    std::string line;
    std::vector<std::string> tokens;
    while(getline(in_file, line)){
        tokenLine(tokens, line);
        std::cout << tokens.size() << "\n";
    }

}

void io::writeNets(char const *file_path)
{
    std::ofstream out_file(file_path, std::ofstream::trunc);
    out_file << "tmp\n";
}

void io::drawNets(char const *input_path, char const *result_path, char const *output_path)
{
    std::string output_name = output_path;
    std::ofstream out_file(output_name + ".gdt", std::ofstream::trunc);
    time_t rawtime;
	struct tm * timeinfo = nullptr;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	char timeseq[25];
	// tm_year is the years counted from 1900 A.D.
	sprintf(timeseq, "%d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	
    // Tokenize the string
    const char *delimiters = "/";
    char *token = strtok((char*)output_path, delimiters);
    char *last_token = nullptr;
    while (token != nullptr) {
        last_token = token;
        token = strtok(nullptr, delimiters);
    }
	// gdt header
    out_file << "gds2{5\n";
    out_file << "m=" << timeseq << " a=" << timeseq << "\n";
    out_file << "lib '" << last_token << "' 0.001 1e-09\n";
    out_file << "cell{c=" << timeseq << " m=" << timeseq << " '" << last_token << "'\n";

    std::ifstream in_file;
    std::string line;
    std::vector<std::string> tokens;
    int inc = 1, track_width = 1, largest_y = 1; // maximum track + 1
    double boundbox_offset = 0.12;

    in_file.open(result_path);
    int now_layer = 0;
    while(getline(in_file, line)){
        tokenLine(tokens, line);
        if(tokens[0] == ".begin"){
            now_layer = std::stoi(tokens[1]);
        }
        else if(tokens[0] == ".V"){
            int below_node_coor_x = std::stoi(tokens[1]), below_node_coor_y = std::stoi(tokens[2]);
            int upper_node_coor_x = std::stoi(tokens[1]), upper_node_coor_y = std::stoi(tokens[3]);
            out_file << "b{" << now_layer << " xy(";
            out_file << below_node_coor_x - boundbox_offset << " " << below_node_coor_y - boundbox_offset << " ";
            out_file << below_node_coor_x + boundbox_offset << " " << below_node_coor_y - boundbox_offset << " ";
            out_file << upper_node_coor_x + boundbox_offset << " " << upper_node_coor_y + boundbox_offset << " ";
            out_file << upper_node_coor_x - boundbox_offset << " " << upper_node_coor_y + boundbox_offset;
            out_file << ")}\n";
            largest_y = std::max(largest_y, stoi(tokens[3]));
        }
        else if(tokens[0] == ".H"){
            int left_node_coor_x = std::stoi(tokens[1]), left_node_coor_y = std::stoi(tokens[2]);
            int right_node_coor_x = std::stoi(tokens[3]), right_node_coor_y = std::stoi(tokens[2]);
            out_file << "b{" << now_layer << " xy(";
            out_file << left_node_coor_x - boundbox_offset << " " << left_node_coor_y - boundbox_offset << " ";
            out_file << right_node_coor_x + boundbox_offset << " " << right_node_coor_y - boundbox_offset << " ";
            out_file << right_node_coor_x + boundbox_offset << " " << right_node_coor_y + boundbox_offset << " ";
            out_file << left_node_coor_x - boundbox_offset << " " << left_node_coor_y + boundbox_offset;
            out_file << ")}\n";
        }
    }
    in_file.close();

    double base_coor_x = 0, base_coor_y = largest_y * track_width, column_index_offset = 0.4;
    double base_column_index_coor_y = base_coor_y + column_index_offset;
    in_file.open(input_path);
    while(getline(in_file, line)){
        tokenLine(tokens, line);
        for(int i = 0; i < static_cast<int>(tokens.size()); i++){
            out_file << "t{" << 215 << " tt1 mc m2 xy(" << base_coor_x + i * inc << " " << base_column_index_coor_y << ") '" << i << "'}\n";
            out_file << "t{" << 215 << " tt1 mc m2 xy(" << base_coor_x + i * inc << " " << base_coor_y << ") '" << tokens[i] << "'}\n";
            out_file << "b{" << tokens[i] << " xy(";
            out_file << base_coor_x + i * inc - boundbox_offset << " " << base_coor_y - boundbox_offset << " ";
            out_file << base_coor_x + i * inc + boundbox_offset << " " << base_coor_y - boundbox_offset << " ";
            out_file << base_coor_x + i * inc + boundbox_offset << " " << base_coor_y + boundbox_offset << " ";
            out_file << base_coor_x + i * inc - boundbox_offset << " " << base_coor_y + boundbox_offset;
            out_file << ")}\n";
        }
        base_coor_y -= largest_y * track_width;
        base_column_index_coor_y -= base_column_index_coor_y + column_index_offset;
    }
    in_file.close();


    

    out_file << "}\n}\n";
    out_file.close();
    
    const char *dl2 = ".";
    char *t = strtok((char*)last_token, dl2);
    char *lt = nullptr;
    while (t != nullptr) {
        lt = t;
        t = strtok(nullptr, delimiters);
        break;
    }
    std::string file_name = lt;
    
    std::string command = "./gdt2gds "+ output_name + ".gdt "+ output_name + ".gds";
    std::cout << command << "\n";
    system(command.c_str());
}