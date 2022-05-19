#include "hash_fun.hpp"
#include <map>

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
        std::filesystem::create_directories(path_pags);
    }
    string nome;
    string path;
    string path_pags;
    string csv;
    string colunas;
    vector<string> pags;
    int qtd_pags = 0;
    int indice_col;
    string col_name;
    map<string, int> nome_para_indice;
    void carregarDados()
    {
        ifstream csv_file(csv);

        // Carregando a linha 1 do csv no map
        string line;
        getline(csv_file, line);
        colunas = line;
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
        ofstream tabela_file((path + "tabela.txt").c_str());
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
        ifstream tabela_file((path + ".tabela.txt").c_str());
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
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path_pags);
        tabela_1.indice_col = tabela_1.nome_para_indice[col_tab_1];
        tabela_2.indice_col = tabela_2.nome_para_indice[col_tab_2];
        // Checando se existe indice pra alguma coluna e definindo relação externa e interna:
        const auto path1 = tabela_1.path + "indice/" + to_string(tabela_1.indice_col);
        const auto path2 = tabela_2.path + "indice/" + to_string(tabela_2.indice_col);
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
    string path = "operacao/";
    string path_pags = "operacao/paginas/";
    int paginas_geradas = 0;
    int tuplas_geradas = 0;
    int numPagsGeradas()
    {
        return paginas_geradas;
    }
    int numIOExecutados()
    {
        return numIOs;
    }
    int numTuplasGeradas()
    {
        return tuplas_geradas;
    }
    void salvarTuplasGeradas(string csv_nome)
    {
        ofstream csv(csv_nome);
        csv << externa.colunas << "," << interna.colunas << '\n';
        ifstream operacao(path + "tabela.txt");
        auto paginas = getline_vector(operacao);
        operacao.close();
        for (int i = 0; i < paginas.size(); i++)
        {
            string pag_name = paginas[i];
            ifstream pag_file((path_pags + pag_name + ".txt").c_str());
            int tuples_amount = count(istreambuf_iterator<char>(pag_file), istreambuf_iterator<char>(), '\n');
            pag_file.clear();
            pag_file.seekg(0);
            for (int j = 0; j < tuples_amount - 1; j++)
            {
                string line;
                getline(pag_file, line);
                csv << line << '\n';
            }
            pag_file.close();
        }
        csv.close();
    }
    void executar()
    {
        string pag_name, chave;
        string tuple_line, tuple2_line;
        int tuples_amount;
        vector<string> tuple_fields;
        // Loop externo - vamos passar de página em página da tabela recuperando uma tupla de cada vez
        int n = this->externa.qtd_pags;
        Diretorio dir(interna.nome, "", interna.indice_col);
        ofstream pag_out_file;
        int pag_out_id = -1, pag_len = PAGE_SIZE;
        numIOs++;
        for (int i = 0; i < n; i++)
        {
            // Pegando o nome da página
            pag_name = this->externa.pags[i];
            // Abrindo página
            ifstream pag_file((this->externa.path_pags + pag_name + ".txt").c_str());
            numIOs++;
            // Descobrindo número de tuplas:
            tuples_amount = count(istreambuf_iterator<char>(pag_file), istreambuf_iterator<char>(), '\n');
            pag_file.clear();
            pag_file.seekg(0);
            // Agora vamos iterar sobre as tuplas da página:
            for (int j = 0; j < tuples_amount; j++)
            {
                // Este vector contem os campos da tuplas
                getline(pag_file, tuple_line);
                tuple_fields = split(tuple_line);
                // Chave de busca
                chave = tuple_fields[externa.indice_col];
                // Busca essa chave nas páginas do bucket
                vector<registro> registros = dir.buscarChave(dir.getBucketHash(chave), chave);
                for (int k = 0; k < registros.size(); k++)
                {
                    int reg_id = stoi(registros[k].id);
                    int pag_id = reg_id / PAGE_SIZE;
                    int reg_pos = reg_id - pag_id * PAGE_SIZE - 1;
                    // Abrindo página da relação interna para adicionar os outros atributos
                    ifstream pag2_file((this->interna.path_pags + to_string(pag_id) + ".txt").c_str());
                    numIOs++;
                    bool encontrou = false;
                    while (!encontrou && getline(pag2_file, tuple2_line))
                    {
                        vector<string> tuple2 = split(tuple2_line);
                        if (tuple2[0] == registros[k].id)
                        {
                            encontrou = true;
                            pag2_file.close();
                        }
                    }
                    if (encontrou)
                    {
                        pag2_file.close();
                        if (pag_len == PAGE_SIZE)
                        {
                            if (pag_out_id > -1)
                            {
                                tuplas_geradas += pag_len;
                                pag_out_file << pag_len << '\n';
                                pag_out_file.close();
                            }
                            pag_len = 0;
                            pag_out_id++;
                            pag_out_file.open(this->path_pags + to_string(pag_out_id) + ".txt");
                            numIOs++;
                        }
                        pag_out_file << tuple_line << "," << tuple2_line << '\n';
                        pag_len++;
                    }
                }
            }
            pag_file.close();
        }
        tuplas_geradas += pag_len;
        pag_out_file << pag_len << '\n';
        pag_out_file.close();
        paginas_geradas = pag_out_id + 1;
        ofstream op_tabela(path + "tabela.txt");
        op_tabela << "0";
        for (int i = 1; i < paginas_geradas; i++)
        {
            op_tabela << "," << i;
        }
        op_tabela << '\n' << paginas_geradas << '\n';
        op_tabela << externa.colunas << "," << interna.colunas << '\n';
        op_tabela.close();
    }
};
