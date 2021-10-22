Funções do programa:


CD  - Troca o diretório atual para o caminho especificado.
      É necessário informar um caminho absoluto começando a partir de /ROOT.DIR/
      Também é possível informar ".." para voltar para o diretório acima.

      Exemplos de execução:

      CD /ROOT.DIR/PASTA1.DIR/
      Troca do diretório atual para /ROOT.DIR/PASTA1.DIR/, assumindo que esse
      diretório existe. Caso não exista o diretório informado a função retorna
      um erro.

      CD ..
      Troca o diretório atual para um diretório um nível acima, por exemplo, se
      o diretório atual for /ROOT.DIR/PASTA.DIR/, troca para /ROOT.DIR/. Se o
      diretório atual for /ROOT.DIR/ a função não faz nada.

DIR - Mostra os conteúdos do diretório atual
