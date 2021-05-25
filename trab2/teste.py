#Define funcoes para verificar a logica de execucao de uma aplicacao leitor/escritor com prioridade para escrita
class LE:
    def __init__(self):
        self.escritores_fila=0
        self.escritores=0
        self.leitores=0

    def leitorBloqueado(self,id):
        '''Recebe o id do leitor. Verifica se a decisao de bloqueio esta correta.'''
        if(self.escritores==0 and self.escritores_fila==0):
            print("ERRO: leitor " + str(id) + " bloqueado quando nao ha escritores ativos e nem na fila!")

    def escritorBloqueado(self,id):
        '''Recebe o id do escritor. Verifica se a decisao de bloqueio esta correta.'''
        if(self.escritores==0) and (self.leitores==0):
            print("ERRO: escritor " + str(id) + " bloqueado quando nao ha escritores nem leitores!")
        self.escritores_fila+=1

    def leitorLendo(self,id):
        '''Recebe o id do leitor, verifica se pode ler e registra que esta lendo.'''
        if(self.escritores>0 or self.escritores_fila>0):
            print("ERRO: leitor " + str(id) + " esta lendo quando ha escritor escrevendo ou em espera!")
        self.leitores+=1

    def escritorEscrevendo(self,id):
        '''Recebe o id do escritor, verifica se pode escrever e registra que esta escrevendo'''
        if(self.escritores>0 or self.leitores>0):
            print("ERRO: escritor " + str(id) + " esta escrevendo quando ha outro escritor ou leitores!")
        self.escritores+=1

    def leitorSaindo(self,id):
        '''Recebe o id do leitor e registra que terminou a leitura.'''
        self.leitores-=1

    def escritorSaindo(self,id):
        '''Recebe o id do escritor e registra que terminou a leitura.'''
        self.escritores-=1

    def escritorDesbloqueado(self, id):
        '''Recebe o id do escritor e registra saida da espera'''
        self.escritores_fila -=1

    def disparandoAlertaVermelho(self,temperaturasMaior35, elementosSensor, alertaVermelho):
        '''Verifica se o alerta foi disparado corretamente'''
        if(temperaturasMaior35<5 or elementosSensor<5 or alertaVermelho==0):
            print("ERRO: alerta vermelho indevido, ha menos que 5 temperaturas registradas maiores que 35 graus ou poucos registros ou o alerta vermelho nao esta ativado")

    def disparandoAlertaAmarelo(self, temperaturasMaior35, elementosSensor, alertaVermelho):
        '''Verifica se o alerta foi disparado corretamente'''
        if(temperaturasMaior35<5 or elementosSensor<5 or alertaVermelho==1):
           print("ERRO: alerta amarelo indevido, ha menos que 5 temperaturas registradas maiores que 35 graus ou poucos registros ou o alerta vermelho esta ativado")

    def disparandoCondicaoNormal(self, temperaturasMaior35, alertaVermelho):
        '''Verifica se o alerta foi disparado corretamente'''
        if(temperaturasMaior35>5 or alertaVermelho==1):
            print("ERRO: alerta condicao normal indevido, ha mais de 5 temperaturas registradas maiores que 35 ou o alerta vermelho esta ativado")

