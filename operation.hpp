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

#define PAGE_SIZE 12

using namespace std;

class Tabela
{
public:
    Tabela() {}
    Tabela(string csv_name)
    {
        nome = split(csv_name, '.')[0];
        path = "./tabelas/" + nome + "/";
        path_pags = path + "/paginas/";
        csv = csv_name;
        std::filesystem::create_directories(path + "/paginas/");
    }
    string nome;
    string path;
    string path_pags;
    string csv;
    vector<string> pags;
    int qtd_pags = 0;
    map<string, int> nome_para_indice;
    void carregarDados()
    {
        ifstream csv_file(csv);

        // Carregando a linha 1 do csv no map
        string line;
        getline(csv_file, line);
        vector<string> headerVec = split(line, ',');
        for (int i = 0; i < headerVec.size(); i++)
        {
            (this->nome_para_indice).insert(pair<string, int>(headerVec[i], i));
        }

        // Transformando as linhas do CSV em páginas
        int pageName = 0;
        int count = 0;
        bool stop = false;
        while (stop == false)
        {

            ofstream page_file(path_pags + to_string(pageName) + ".txt");

            while (count < PAGE_SIZE && getline(csv_file, line))
            {
                page_file << line << '\n';
                count++;
            }
            if (count > 0)
            {
                this->pags.emplace_back(to_string(pageName));
                this->qtd_pags++;
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
        this->salvar_tabela();
    }
    void salvar_tabela()
    {
        ofstream tabela_file((path + nome + "tabela.txt").c_str());
        for (int i = 0; i < this->pags.size(); i++)
        {
            tabela_file << this->pags[i];
            if (i < this->pags.size() - 1)
                tabela_file << ',';
        }
        tabela_file << endl;
        tabela_file << to_string(this->qtd_pags) << endl;
        int cont = 1;
        for (auto const &x : this->nome_para_indice)
        {
            tabela_file << x.first << ':' << x.second;
            cont++;
            if (cont <= this->nome_para_indice.size())
                tabela_file << ',';
        }
    }
    void carregar_tabela()
    {
        ifstream tabela_file((path + nome + ".tabela.txt").c_str());
        string line;

        this->pags = getline_vector(tabela_file);
        getline(tabela_file, line);
        this->qtd_pags = stoi(line);
        vector<string> mappings = getline_vector(tabela_file);
        size_t doisPs;
        string key, value;
        for (int i = 0; i < mappings.size(); i++)
        {
            doisPs = mappings[i].find(":");
            key = mappings[i].substr(0, doisPs + 1);
            value = mappings[i].substr(doisPs + 1, mappings[i].size() - 1 - doisPs);
            this->nome_para_indice.insert(make_pair(key, stoi(value)));
        }
    }

    void printTable()
    {

        for (int i = 0; i < this->pags.size(); i++)
        {
            cout << this->pags[i] << ',';
        }
        cout << endl;

        cout << this->qtd_pags << endl;

        cout << "\nThe nome_para_indice map is : \n";
        cout << "\tKEY\tELEMENT\n";
        for (auto itr = this->nome_para_indice.begin(); itr != this->nome_para_indice.end(); ++itr)
        {
            cout << '\t' << itr->first << '\t' << itr->second
                 << '\n';
        }
        cout << endl;
    }
};

class Operador
{
public:
    Operador(Tabela tabela_1, Tabela tabela_2, string col_tab_1, string col_tab_2)
    {
        auto it1 = tabela_1.nome_para_indice.find(col_tab_1);
        int tab_1_pos = it1->second;

        auto it2 = tabela_2.nome_para_indice.find(col_tab_2);
        int tab_2_pos = it2->second;

        // Checando se existe indice pra alguma coluna e definindo relação externa e interna:
        const auto path1 = tabela_1.path + "indice/" + to_string(tab_1_pos);
        const auto path2 = tabela_2.path + "indice/" + to_string(tab_2_pos);
        if (std::filesystem::is_directory(path1))
        {
            this->interna = tabela_1;
            this->externa = tabela_2;
            this->viabilidade = true;
        }
        else if (std::filesystem::is_directory(path2))
        {
            this->interna = tabela_2;
            this->externa = tabela_1;
            this->viabilidade = true;
        }
        else
        {
            cout << "Não há índices em nenhum dos atributos escolhidos" << endl;
            this->viabilidade = false;
        }
        cout << "Viabilidade da operação = " << viabilidade << endl;
        if (viabilidade)
        {
            cout << "Relação externa é " << externa.nome << endl;
            cout << "Relação interna é " << interna.nome << endl;
        }
    }
    Tabela externa;
    Tabela interna;
    bool viabilidade;

    void executar()
    {
        string pag_name;
        string line;
        int tuples_amount;
        vector<string> tuple_fields;
        // Loop externo - vamos passar de página em página da tabela recuperando uma tupla de cada vez
        int n = this->externa.qtd_pags;

        for (int i = 0; i < n; i++)
        {
            // Pegando o nome da página
            pag_name = this->externa.pags[i];
            // Abrindo página
            ifstream pag_file((this->externa.path + to_string(i) + ".txt").c_str());

            // Descobrindo número de tuplas:
            tuples_amount = count(istreambuf_iterator<char>(pag_file), istreambuf_iterator<char>(), '\n');
            pag_file.clear();
            pag_file.seekg(0);
            // Agora vamos iterar sobre as tuplas da página:
            for (int j = 0; j < tuples_amount; j++)
            {
                // Este vector contem os campos da tuplas
                tuple_fields = getline_vector(pag_file);
            }

            pag_file.close();
        }
    }
};
