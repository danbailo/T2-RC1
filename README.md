# Trabalho 2 - Redes de Computadores I

## Descrição

Servidor de Repasse:
* Deverá haver 2 ou mais servidores com informações distribuídas entre eles. Exemplo: Se um cliente fizer uma consulta a uma informação no Servidor (S1) e o mesmo não tiver a informação, o S1 deverá consultar os demais servidores para verificar qual dos servidores tem a informação. Caso a informação não esteja em nenhum deles, uma mensagem de erro deverá ser gerada.
* Toda vez que o servidor não tiver o arquivo, o mesmo deverá consultar nos demais servidores antes de disparar a mensagem de erro.
* Linguagem C ou C++.
* Individual ou Grupo.
* Um relatório deverá ser entregue para a avaliação. Bem como uma apresentação demonstrando o funcionamento da aplicação.

---
## Requisitos

* `g++`
  
---

## Como usar

Para executar o programa, ainda com o terminal/prompt de comando aberto, siga os passos abaixo:
* `sh script.sh`
* `cd bin/`
* `./server1 1234 1235`
* `./client 1234`

---

### Agradecimentos

Gostaria de agradecer ao meu amigo Leandro Viana([@leandrocodes](https://github.com/leandrocodes/)) por me disponibilizar seu código para que eu pudesse aprender/entender a lógica da conexão de dois servidores para que um pudesse "pedir" para o outro caso este não houvesse tal informação, graças a ele, consegui implementar a minha versão do trabalho.