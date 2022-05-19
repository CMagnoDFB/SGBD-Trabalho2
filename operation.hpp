#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <bitset>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>

using namespace std;

class Tabela
{
public:
    Tabela(string csv_name)
    {
        size_t d = csv_name.size() - 4;
        nome = csv_name.substr(0, d);
        path = "./tabelas/" + nome + "/";
    }
    string nome;
    string path;
    vector<string> pags;
    int qtd_pags;
    map<string, int> nome_para_indice;

    void carregarDados()
    {
        ifstream csv_file(this->nome + ".csv");

        // Carregando a linha 1 do csv no map
        string line;
        getline(csv_file, line);
        vector<string> headerVec = split(line, ',');
        for (int i = 0; i < headerVec.size(); i++)
        {
            (this->nome_para_indice).insert(pair<string, int>(headerVec[i], i));
        }
        // teste
        // map<string, int>::iterator itr;
        // cout << "\nThe map gquiz1 is : \n";
        // cout << "\tKEY\tELEMENT\n";
        // for (itr = this->nome_para_indice.begin(); itr != this->nome_para_indice.end(); ++itr)
        // {
        //     cout << '\t' << itr->first << '\t' << itr->second
        //          << '\n';
        // }
        // cout << endl;

        // Transformando as linhas do CSV em páginas
        int pageName = 0;
        int count = 0;
        bool stop = false;
        while (stop == false)
        {

            ofstream page_file(this->path + to_string(pageName) + ".txt");

            while (count < 12 && getline(csv_file, line))
            {
                page_file << line << '\n';
                count++;
            }
            if (count > 0)
            {
                this->pags.emplace_back(to_string(pageName));
                pageName++;
                page_file << to_string(count);
                page_file.close();
                count = 0;
            }
            if (!csv_file.good())
            {
                stop = true;
            }
        }
    }
};

class Pagina
{
public:
};
