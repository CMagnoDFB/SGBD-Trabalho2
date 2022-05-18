# SGBD-Hash
c++ -o main.exe main.cpp -std=c++17
Link da aula de tira-duvidas:
https://drive.google.com/file/d/1VITYUzVbpqevmfMwuYHHUJ-_jNFfgG0x/view?usp=sharing

Duvida tirada com o Daniel via e-mail:
Boa tarde, Daniel.
Queria tirar 02 dúvidas sobre hash:
1- Suponha que o in.txt possui uma linha que diz INC:1999. Então o que temos que fazer é buscar no arquivo .csv todos os vinhos que possuem este ano de colheita e adicionar cada um deles no bucket apropriado (com os seus respectivos ids)?

2- De maneira complementar, se o in.txt tem uma linha que diz BUS=:1999, então temos que buscar o bucket apropriado e retornar quantos registros haviam lá com este ano de colheita?

Dúvida extra: os arquivos buckets necessariamente precisam ser em formato de texto? Pergunto pois tive a ideia de tratá-los como objetos e usar serialização e desserialização para facilitar a leitura e gravação deles.

Obrigado.

Resposta:
Oi, Carlos, tudo bem?!

Vamos por parte.

1. Isso mesmo, Carlos. Usando seu exemplo, suponha que duas tuplas possuem ano_colheita 1999 na tabela Vinhos. Portanto, INC:1999 vai fazer com que duas entradas no índice sejam adicionadas. Importante: a tabela Vinhos é estática, ou seja, as suas tuplas não vão ser modificadas. O que é dinâmico, na verdade, é o processo de criação do índice. Certo?

2. Correto. Você vai realizar a busca em Vinhos usando o índice, não diretamente. Então, vamos supor que o arquivo in.txt possui somente o comando de busca, logo nenhum registro vai ser encontrado, correto?! Agora suponha que antes desse comando de busca no in.txt, exista o comando de inserção do item anterior. Nesse caso, agora o retorno será composto por duas tuplas, entende?

Não, a organização do arquivo pode ser escolhido por você. Não há nenhuma restrição quanto a isso, certo? Importante notar que o único ponto é que somente uma página por vez pode estar na memória, no seu caso um bucket por vez, além do diretório. Certo?

Qualquer dúvida, fique à vontade,

Daniel.
