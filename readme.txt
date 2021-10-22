Como utilizar o programa:
1 - Crie o arquivo binário LIGHTFS.BIN com o programa GEN.EXE.
2 - Execute o programa CLI.EXE para manipular o arquivo LIGHTFS.BIN.



Funções do programa:

CD     - Troca o diretório atual para o caminho especificado.
         Atenção: É necessário informar um caminho absoluto começando a partir
         de /ROOT.DIR/
         *Também é possível informar ".." para voltar para o diretório acima.

         Exemplos de execução:

         /ROOT.DIR/>CD /ROOT.DIR/PASTA.DIR/

         /ROOT.DIR/PASTA.DIR/>CD ..
         Nesse exemplo, troca para o diretório /ROOT.DIR/.
         *Se o diretório atual for /ROOT.DIR/, o programa não faz nada.


CLS    - Comando para limpar a tela.
         
         Exemplo de execução:

         /ROOT.DIR/>CLS


DIR    - Mostra o conteúdo do diretório atual.
         Se o diretório estiver vazio, o programa imprime "<vazio>"

         Exemplos de execução:

         /ROOT.DIR/PASTA.DIR/>DIR

         /ROOT.DIR/>DIR


DISP   - Mostra o conteúdo de um arquivo dentro do diretório atual.
         Se o arquivo estiver vazio, o programa imprime "<Arquivo vazio>".
         Se o arquivo informado for um diretório, o comando é equivalente a DIR
         dentro do diretório informado. (Diretórios também são arquivos)

         Exemplos de execução:

         /ROOT.DIR/>DISP ARQUIVO.TXT

         /ROOT.DIR/>DISP PASTA.DIR


EDIT   - Edita o conteúdo de um arquivo. Se o arquivo informado for um
         diretório, o programa retorna um erro.

         Exemplo de execução:

         /ROOT.DIR/>EDIT ARQUIVO2.TXT "The quick brown fox jumps over the lazy dog."


HELP   - Mostra uma tela com todos os comandos e uma breve explicação de cada
         um dos comandos disponíveis no programa.


MKDIR  - Cria um diretório com o nome especificado dentro do diretório atual.
         Atenção: O nome do diretório precisa ter no máximo 8 caracteres.
 
         Exemplo de execução:

         /ROOT.DIR/>MKDIR FOLDER

         
MKFILE - Cria um arquivo com o nome e extensão especificados dentro do
         diretório atual. Caso a extensão especificada seja "DIR", o comando é
         equivalente a MKDIR. (Diretórios também são arquivos)
         Atenção: O nome do arquivo precisa ter no máximo 8 caracteres e a
         extensão precisa ter exatamente 3 caracteres.

         Exemplos de execução:

         /ROOT.DIR/>MKFILE FILE.TXT

         /ROOT.DIR/>MKFILE FOLDER.DIR


MOVE   - Move um arquivo de um diretório para outro. É necessário informar o
         caminho absoluto do arquivo a ser movido e do destino.

         Exemplos de execução:

         /ROOT.DIR/>MOVE /ROOT.DIR/ARQUIVO.TXT /ROOT.DIR/PASTA.DIR

         /ROOT.DIR/>MOVE /ROOT.DIR/PASTA.DIR/ARQUIVO.TXT /ROOT.DIR/FOLDER.DIR
         *note que não é necessário estar nem no diretório de origem,
         nem no diretório de destino para realizar o comando.


RENAME - Renomeia um arquivo dentro do diretório atual. Caso a extensão do
         arquivo informado for "DIR", não é permitido trocar a extensão.
         Também não é permitido trocar a extensão de um arquivo de texto para "DIR".
         Atenção: O nome do arquivo precisa ter no máximo 8 caracteres e a
         extensão precisa ter exatamente 3 caracteres.

         Exemplos de execução:
         
         /ROOT.DIR/>RENAME ARQUIVO.TXT FILE.TXT

         /ROOT.DIR/>RENAME FILE.TXT FILE.DOC

         /ROOT.DIR/>RENAME PASTA.DIR FOLDER.DIR


RM     - Remove um arquivo dentro do diretório atual. Para remover um diretório,
         é necessário que o diretório esteja vazio.

         Exemplos de execução:

         /ROOT.DIR/>RM ARQUIVO.TXT

         /ROOT.DIR/>RM PASTA.DIR
         *note que se o diretório não estiver vazio, o programa retorna um erro.


EXIT   - Termina a execução e fecha o programa.

         Exemplo de execução:

         /ROOT.DIR/>EXIT
