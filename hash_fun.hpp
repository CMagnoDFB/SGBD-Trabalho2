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

#define PAGE_SIZE 12
#define BUCKET_MAX_SIZE 200
#define OP_INC 0
#define OP_REM 1
#define OP_BUS 2

int numIOs = 0;

using namespace std;
// Faz split da string por espaços
vector<string> split(string str, char delimiter = ',')
{
    stringstream stream(str);
    vector<string> result;
    string word;
    while (getline(stream, word, delimiter))
    {
        result.push_back(word);
    }
    return result;
}
vector<string> getline_vector(istream &x, char delimiter = ',')
{
    string line;
    getline(x, line);
    return split(line, delimiter);
}
// Essa funcao recebe a chave, passa na funcao hash e devolve os ultimos prof_global bits
string getHash(int chave, int prof_global)
{
    int res = chave;
    string raw = bitset<32>(res).to_string();
    string fin = raw.substr(raw.size() - prof_global);
    return fin;
}
string getHash(string chave, int prof_global)
{
    return getHash(stoi(chave), prof_global);
}
struct registro
{
    string chave;
    string id;
};
// As celulas compoem o diretorio. O bucket_file e o nome do arquivo, simulando um ponteiro.
struct celula
{
    string hash;
    string bucket_file;
};
class Bucket
{
public:
    // Construtor
    Bucket()
    {
    }

    string nome;
    int full = 0;
    int prof_local = 2;
    int len = 0;

    vector<registro> regs;
};
ofstream out("out.txt");
class Diretorio
{
public:
    Diretorio(string tab_nome, string csv, int indice)
    {
        dir.resize(4);
        dir[0] = {"00", "00"};
        dir[1] = {"01", "01"};
        dir[2] = {"10", "10"};
        dir[3] = {"11", "11"};
        nome = csv;
        path = "./tabelas/" + tab_nome + "/indice/" + to_string(indice) + "/";
        indice_col = indice;
        out << "[OPERACOES NO INDICE DA TABELA " + tab_nome + "]\n";
        construir();
    }
    int prof_global = 2;        //
    string nome = "vinhos.csv"; // Nome da tabela
    string path = "./tabelas/vinho/indice/2/";
    int indice_col;
    vector<struct celula> dir;
    void iniciar_buckets()
    {
        for (int i = 0; i < dir.size(); i++)
        {
            Bucket bucket;
            bucket.nome = dir[i].bucket_file;
            salvar_bucket(bucket);
        }
    }
    void expandir()
    {
        prof_global++;
        int new_size = 1 << prof_global;
        int middle = new_size / 2;
        dir.resize(new_size);
        for (int i = 0; i < middle; i++)
            dir[i + middle] = dir[i];
        // Ate aqui duplicamos os nomes dos buckets, falta adicionar um 0 na primeira metade e um 1 na segunda metade.
        for (int i = 0; i < middle; i++)
            dir[i].hash = "0" + dir[i].hash;
        for (int i = middle; i < new_size; i++)
            dir[i].hash = "1" + dir[i].hash;
    }
    void mostrar_profundidade()
    {
        out << "P:/" << prof_global << std::endl;
    }
    // Funcao que lida com operacoes
    void operar(int op, string chave)
    {
        string bucketHash;
        vector<registro> registros;
        vector<string> idsToAdd;
        vector<string> idsRemoved;
        Bucket bucket;
        switch (op)
        {
        // INC
        case OP_INC:
            idsToAdd = Diretorio::getFromCSV(nome, chave, indice_col);
            //  Para cada ID, fazer a operação de inclusão (aqui o ano é o mesmo)
            bucketHash = getBucketHash(chave);
            registros = buscarChave(bucketHash, chave);
            if (registros.size() > 0)
                break;
            for (int i = 0; i < idsToAdd.size(); i++)
            {
                bucketHash = getBucketHash(chave);
                addRegistro(bucketHash, chave, idsToAdd[i]);
            }
            bucketHash = getBucketHash(chave);
            bucket = carregar_bucket(bucketHash);
            out << "INC:" << chave << "/" << idsToAdd.size() << "," << prof_global << "," << bucket.prof_local << endl;
            break;
        // REM
        case OP_REM:
            bucketHash = getBucketHash(chave);
            bucket = carregar_bucket(bucketHash);
            idsRemoved = removerChave(bucketHash, chave);
            out << "REM:" << chave << "/" << idsRemoved.size() << "," << prof_global << "," << bucket.prof_local << endl;
            break;
        // BUS
        case OP_BUS:
            bucketHash = getBucketHash(chave);
            registros = buscarChave(bucketHash, chave);
            out << "BUS:" << chave << "/" << registros.size() << endl;
            break;
        }
    }
    // Recebe-se a chave e retorna-se o bucket adequado
    string getBucketHash(string chave)
    {
        string hash = getHash(chave, prof_global);
        int pos = stoi(hash, 0, 2);
        return dir[pos].bucket_file;
    }
    // Essa funcao considera que a string hash contem o nome de um arquivo bucket existente
    void addRegistro(string hash, string chave, string id)
    {
        // Agora devemos abrir o arquivo bucket e fazer a insercao
        Bucket bucket = carregar_bucket(hash);
        registro newR;
        newR.chave = chave;
        newR.id = id;
        // Verificando se o bucket esta cheio:
        if (bucket.full == 0)
        {
            bucket.regs.emplace_back(newR);
            bucket.len++;
            if (bucket.len == BUCKET_MAX_SIZE)
                bucket.full = 1;
            salvar_bucket(bucket);
        }
        // Caso o bucket esteja cheio, precisamos realizar um bucket split.
        // Mas primeiro, precisamos saber se o diretorio comporta esse split.
        else
        {
            bool expandir_diretorio = !(bucket.prof_local < prof_global);
            // Bucket split e possivel sem expandir o diretorio?
            if (expandir_diretorio)
                expandir();
            // Na pratica teremos dois buckets vazios agora (com hash 1 bit mais longo), e entao faremos a redistribuicao dos registros.
            int oldBitSize = bucket.prof_local;

            // Vamos esvaziar o bucket original para distribuir os registros entre ele e o bucket split.
            bucket.prof_local++;
            bucket.nome = "0" + bucket.nome;
            bucket.full = 0;
            out << "DUP_DIR:/" << prof_global << "," << bucket.prof_local << endl;
            // Criando novo bucket:
            Bucket splitBucket;
            splitBucket.nome = "1" + hash;
            splitBucket.prof_local = bucket.prof_local;
            splitBucket.len = 0;
            splitBucket.full = 0;
            // Aqui fazemos a redistribuicao dos registros:
            // Criamos um vector temporario para guardar todos os registros
            vector<registro> temp = bucket.regs;
            // Sabendo os nomes dos buckets novos, vamos fazer a atualizacao do diretorio.
            atualizar_diretorio(bucket.nome, splitBucket.nome);
            // Limpamos o vector do bucket original.
            bucket.regs.clear();
            bucket.len = 0;
            // Deletando bucket antigo:
            remove((path + hash + ".txt").c_str());
            // Salvamos os buckets esvaziados em arquivo:
            salvar_bucket(bucket);
            salvar_bucket(splitBucket);
            // Agora iteramos sobre o vector temp e chamamos a funcao recursivamente (nao esquecer do registro novo):
            for (int i = 0; i < temp.size(); i++)
            {
                addRegistro((getHash(temp[i].chave, oldBitSize + 1)), temp[i].chave, temp[i].id);
            }
            addRegistro((getHash(chave, oldBitSize + 1)), chave, id);
        }
    }
    // Essa funcao retorna os registros com chave igual à de busca
    vector<registro> buscarChave(string hash, string chave)
    {
        vector<registro> registros;
        // Agora devemos abrir o arquivo bucket e fazer a busca
        Bucket bucket = carregar_bucket(hash);
        for (auto registro : bucket.regs)
            if (registro.chave == chave)
                registros.emplace_back(registro);
        numIOs += 1 + (bucket.len - 1) / PAGE_SIZE;
        return registros;
    }
    // Essa funcao remove os registros com chave igual à da pesquisa e retorna os ids removidos
    vector<string> removerChave(string hash, string chave)
    {
        vector<string> ids;
        vector<registro> temp;
        // Agora devemos abrir o arquivo bucket e remover
        Bucket bucket = carregar_bucket(hash);
        for (auto registro : bucket.regs)
            if (registro.chave == chave)
                ids.emplace_back(registro.id);
            else
                temp.emplace_back(registro);
        bucket.regs = temp;
        bucket.len = temp.size();
        salvar_bucket(bucket);
        return ids;
    }
    // Retona o vetor de ids a serem adicionados com aquela chave.
    static vector<string> getFromCSV(string tab, string chave, int indice_col)
    {
        ifstream csv(tab);
        vector<string> ids, data;
        string line, id, atributo;
        // Pulando a primeira linha
        getline(csv, line);
        // Percorrendo demais linhas
        while (getline(csv, line))
        {
            data = split(line);
            if (data[indice_col] == chave)
                ids.emplace_back(data[0]);
        }
        return ids;
    }
    void construir()
    {
        ifstream diretorio((path + "diretorio.txt").c_str());
        std::filesystem::create_directories(path);
        if (diretorio.good())
        {
            diretorio.close();
            carregar_diretorio();
            return;
        }
        salvar_diretorio();
        iniciar_buckets();
        ifstream csv(nome);
        vector<string> data;
        string line, id, chave, bucketHash;
        // Pulando a primeira linha
        getline(csv, line);
        // Percorrendo demais linhas
        while (getline(csv, line))
        {
            data = split(line);
            id = data[0];
            chave = data[indice_col];
            bucketHash = getBucketHash(chave);
            addRegistro(bucketHash, chave, id);
        }
        salvar_diretorio();
    }
    // Esta funcao recebe um objeto bucket e salva em um arquivo
    void salvar_bucket(Bucket bucket)
    {
        //  Removendo arquivo com nome anterior (mesmo que o novo bucket nao tenha mudado de nome)
        remove((path + bucket.nome + ".txt").c_str());
        // Criando novo arquivo:
        ofstream bucket_file(path + bucket.nome + ".txt");
        bucket_file << "full," << bucket.full << endl;
        bucket_file << "prof_local," << to_string(bucket.prof_local) << endl;
        bucket_file << "len," << to_string(bucket.len) << endl;
        for (int i = 0; i < bucket.len; i++)
            bucket_file << bucket.regs[i].chave << "," << bucket.regs[i].id << endl;
        bucket_file << "end";
        bucket_file.close();
    }
    // Esta funcao recebe um nome de arquivo, le e retorna um objeto bucket correspondente
    Bucket carregar_bucket(string file_name)
    {
        ifstream bucket_file((path + file_name + ".txt"));
        Bucket bucket;
        // Pegando dados
        bucket.nome = file_name;
        // Depois
        bucket.full = stoi(getline_vector(bucket_file)[1]);
        bucket.prof_local = stoi(getline_vector(bucket_file)[1]);
        bucket.len = stoi(getline_vector(bucket_file)[1]);
        string chave, id;
        vector<string> data;
        registro newR;
        for (int i = 0; i < bucket.len; i++)
        {
            data = getline_vector(bucket_file);
            chave = data[0];
            id = data[1];
            newR.id = id;
            newR.chave = chave;
            bucket.regs.emplace_back(newR);
        }
        bucket_file.close();
        return bucket;
    }
    // Funcao que atualiza o diretorio apos um bucket split.
    // Exemplo: situação inicial -> Diretorio com hash 010 e 110 ambos apontam para o mesmo bucket, o bucket 10.
    // Digamos que o bucket 10 sofra split. Nesse caso agora vão existir dois arquivos, o arquivo 010 e 110.
    // Agora as celulas do diretorio com hash 010 e 110 devem "apontar" (ter como bucket_file) para arquivos distintos.
    // Para isso, basta verificar as celulas do diretorio que tem hash terminando em 010 (que podem ser varias tambem, dependendo da profundidade global) e colocar seu ponteiro como sendo 010.
    // Faz-se o mesmo procedimento para o valor 110.
    void atualizar_diretorio(string bucketFileName, string splitBucketFileName)
    {
        int t = bucketFileName.size();
        string temp;
        for (int i = 0; i < dir.size(); i++)
        {
            temp = dir[i].hash;
            if (temp.substr(temp.size() - t) == bucketFileName)
                dir[i].bucket_file = bucketFileName;
            else if (temp.substr(temp.size() - t) == splitBucketFileName)
                dir[i].bucket_file = splitBucketFileName;
        }
    }
    void salvar_diretorio()
    {
        mostrar_profundidade();
        remove((path + "diretorio.txt").c_str());
        ofstream diretorio((path + "diretorio.txt").c_str());
        diretorio << prof_global << endl;
        for (int i = 0; i < dir.size(); i++)
            diretorio << dir[i].bucket_file << endl;
        diretorio.close();
    }
    void carregar_diretorio()
    {
        ifstream diretorio((path + "diretorio.txt").c_str());
        diretorio >> prof_global;
        int size = 1 << prof_global;
        dir.resize(size);
        for (int i = 0; i < size; i++)
        {
            dir[i].hash = getHash(i, prof_global);
            diretorio >> dir[i].bucket_file;
        }
        diretorio.close();
    }
};
