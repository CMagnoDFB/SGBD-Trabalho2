#include <new>
#include "hash_fun.hpp"
#include "operation.hpp"

using namespace std;

int main(int argc, char **argv)
{
    Diretorio dir1("vinho", "vinho.csv", 3);
    Diretorio dir2("vinho", "vinho.csv", 4);
    Diretorio dir3("uva", "uva.csv", 4);
    Tabela vinho{"vinho.csv"}; // cria estrutura necessaria para a tabela
    Tabela uva{"uva.csv"};
    Tabela pais{"pais.csv"};

    // vinho.carregarDados(); // le os dados do csv e add na estrutura da tabela, caso necessario
    uva.carregarDados();
    pais.carregarDados();
    vinho.carregarDados();
    Operador op{vinho, uva, "pais_producao_id", "tipo"};

    return 0;
}