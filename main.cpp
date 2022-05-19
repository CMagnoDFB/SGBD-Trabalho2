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
    Operador op1{uva, pais, "pais_origem_id", "pais_id"};
    Operador op2{uva, vinho, "uva_id", "uva_id"};
    Operador op3{vinho, pais, "pais_producao_id", "pais_id"};
    // Operador op{uva, vinho, "uva_id", "uva_id"};
    // Operador op{vinho, pais, "pais_producao_id", "pais_id"};
    op1.executar();
    op2.executar();
    op3.executar();
    cout << "#Pags: " << op1.numPagsGeradas();     // Retorna a quantidade de paginas geradas pela operacao
    cout << "\n#IOss: " << op1.numIOExecutados();  // Retorna a quantidade de IOs geradas pela operacao
    cout << "\n#Tups: " << op1.numTuplasGeradas(); // Retorna a quantidade de tuplas geradas pela operacao
    op1.salvarTuplasGeradas("juncao.csv");         // Retorna as tuplas geradas pela operacao e salva em um csv
    cout << "\n#Pags: " << op2.numPagsGeradas();     // Retorna a quantidade de paginas geradas pela operacao
    cout << "\n#IOss: " << op2.numIOExecutados();  // Retorna a quantidade de IOs geradas pela operacao
    cout << "\n#Tups: " << op2.numTuplasGeradas(); // Retorna a quantidade de tuplas geradas pela operacao
    op2.salvarTuplasGeradas("juncao2.csv");         // Retorna as tuplas geradas pela operacao e salva em um csv
    cout << "\n#Pags: " << op3.numPagsGeradas();     // Retorna a quantidade de paginas geradas pela operacao
    cout << "\n#IOss: " << op3.numIOExecutados();  // Retorna a quantidade de IOs geradas pela operacao
    cout << "\n#Tups: " << op3.numTuplasGeradas(); // Retorna a quantidade de tuplas geradas pela operacao
    op3.salvarTuplasGeradas("juncao3.csv");         // Retorna as tuplas geradas pela operacao e salva em um csv
    return 0;
}