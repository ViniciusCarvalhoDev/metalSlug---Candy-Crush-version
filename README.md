# Metal Slug Candy Crush Version

![banner](https://user-images.githubusercontent.com/51410946/116156666-2a641380-a6c2-11eb-8af2-903eda174635.png)

Game desenvolvido na disciplina de programação e desenvolvimento de software 1. Utilizando C (ISSO MESMO APENAS C) e a lib Allegro 5.

Para fazer o game rodar é preciso apenas clicar no arquivo candyc.exe 
<strong align="center">A resolução recomendada é 1920x1080 qualquer resolução abaixo irá causar distorções e mal funcionamento do jogo. Caso o jogo não rode você pode baixar a dll do allegro nesse link https://drive.google.com/file/d/1W8jWD-mWBhov07-qugFGPPkWAOGF5h0Z/view?usp=sharing</strong>

Trabalho Prático - Candy Crush
Pedro O.S. Vaz de Melo
October 18, 2019

October 18, 2019
1 Descrição do Problema
O objetivo deste trabalho é fazer com que o aluno utilize as técnicas de programação
aprendidas na disciplina para desenvolver um jogo eletrôni
semelhante ao Candy Crush. Nesse jogo, doces de diversos tipos são espalhados
pela tela, na forma de um grid (ou matriz). O objetivo do jogador é movimentar
os doces e formar sequências de doces do mesmo tipo que tenham
tamanho maior ou igual a três. As sequencias podem ser formadas tanto nas
linhas quanto nas colunas da matriz. A movimentaçãoo dos doces é feita a partir
de trocas: o jogador clica no doce que quer movimentar e, mantendo o botão do
mouse pressionado, arrasta o cursor para o doce alvo, ou seja, aquele que deve
ser trocado de posição com o doce clicado inicialmente. Trocas só podem ser
feitas com doces vizinhos. Doces vizinhos são aqueles que estão imediatamente
acima, abaixo, à direita ou à esquerda do doce alvo. Ao formar uma sequência,
os doces são consumidos pelo jogador, gerando pontos para ele. O consumo
imediato de n doces vale mais pontos que o consumo dos n doces em instantes
separados. Exemplo: um movimento que consume 6 doces imediatamente vale
mais pontos que dois movimentos que consome, em cada um deles, 3 doces. O
jogador tem direito a um conjunto finito de movimentos e o jogo termina quando
os seus movimentos disponíveis acabarem.
Assim, ao implementar o jogo, você deverá registrar e exibir algum tipo de
pontuação e também o núumero de movimentos disponíveis para o jogador. Ao
final do jogo deverá ser exibida uma tela informando a pontuacão do usuário
e o recorde atual. Caso a pontuaçãoo do usuário seja maior que o recorde atual,
um texto com essa informação deve ser exibido para o usuário e um novo
recorde deve ser registrado. Este trabalho tem um valor total de 20 pontos.
Execute os arquivos candyc.exe para um exemplo de jogo que pode ser implementado.
A versão clássica do jogo pode ser jogada em https://king.com/
game/candycrush.
