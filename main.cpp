#include <new>
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

    uva.carregarDados(); // le os dados do csv e add na estrutura da tabela, caso necessario
    pais.carregarDados();
    vinho.carregarDados();
    Operador op{uva, pais, "pais_origem_id", "pais_id"};
    //Operador op{uva, vinho, "uva_id", "uva_id"};
    //Operador op{vinho, pais, "pais_producao_id", "pais_id"};
    op.executar();
    cout << "#Pags: " << op.numPagsGeradas();     // Retorna a quantidade de paginas geradas pela operacao
    cout << "\n#IOss: " << op.numIOExecutados();  // Retorna a quantidade de IOs geradas pela operacao
    cout << "\n#Tups: " << op.numTuplasGeradas(); // Retorna a quantidade de tuplas geradas pela operacao
    op.salvarTuplasGeradas("juncao.csv"); // Retorna as tuplas geradas pela operacao e salva em um csv
    return 0;
}