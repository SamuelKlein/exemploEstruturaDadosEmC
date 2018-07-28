#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

typedef struct {
	long codigo;
	char nome[ 50 ];
	char telefone[ 11 ];
	char email[ 50 ];
	float peso;
	float altura;

	long posEsq;
	long posDir;
} paciente;

FILE *arqDados;

int editaPaciente( paciente *pac, int inserir ) {
	int opcao;
	float nro;

	while( 1 ) {
	    clrscr();
	    if( inserir == 0 ){
	    	gotoxy( 5, 3 ); printf( "1 - Codigo....: %ld", pac->codigo );
	    } else {
	    	gotoxy( 5, 3 ); printf( "1 - Codigo....: %ld ( sem permisão ) ", pac->codigo );
	    }
	    gotoxy( 5, 4 ); printf( "2 - Nome......: %s", pac->nome );
	    gotoxy( 5, 5 ); printf( "3 - Telefone..: %s", pac->telefone );
	    gotoxy( 5, 6 ); printf( "4 - eMail.....: %s", pac->email );
	    gotoxy( 5, 7 ); printf( "5 - Peso......: %10.2f", pac->peso );
	    gotoxy( 5, 8 ); printf( "6 - Altura....: %10.2f", pac->altura );

	    gotoxy( 5, 10 ); printf( "Indique o campo a alterar. S para salvar ou X para cancelar " );
	    gotoxy( 5, 70 );
	    opcao = getch();

	    switch( opcao ) {
		case '1':
			if( inserir == 0 ){
				gotoxy( 5, 12 );
				printf( "Codigo: " );
				scanf( "%ld", &pac->codigo );
			} else {
				gotoxy( 5, 12 );
				printf( "proibido Altera Codigo do Paciente" );
				getch();
			}
		    break;
		case '2':
		    gotoxy( 5, 12 );
		    printf( "Nome: " );
//		    flushall();
		    scanf( "%s", pac->nome );
//		    getstr( pac->nome );
		    break;
		case '3':
		    gotoxy( 5, 12 );
		    printf( "Telefone: " );
		    scanf( "%s", pac->telefone );
//		    flushall();
//		    gets( pac->telefone );
		    break;
		case '4':
		    gotoxy( 5, 12 );
		    printf( "eMail: " );
		    scanf( "%s", pac->email );
//		    flushall();
//		    gets( pac->email );
		    break;
		case '5':
		    gotoxy( 5, 12 );
		    printf( "Peso: " );
		    scanf( "%f", &nro );
		    pac->peso = nro;
		    break;
		case '6':
		    gotoxy( 5, 12 );
		    printf( "Altura: " );
//		    flushall();
		    scanf( "%f", &nro );
		    pac->altura = nro;
		    break;
		case 's':
		case 'S': return 1;
		case 'x':
		case 'X': return 0;
	    }
	}
}

long posicionaNoRegistro( long nrRegistro ) {
	long posicao = ( nrRegistro - 1 ) * sizeof( paciente );

	fseek( arqDados, posicao, SEEK_SET );
	return posicao;
}

paciente buscaArvore( long codigo ){
	paciente pac;
	if ( fread( &pac, sizeof( paciente ), 1, arqDados ) == 1 ){
		if( codigo > pac.codigo ) {
			if( pac.posDir != -1 ){
				posicionaNoRegistro( pac.posDir );
				buscaArvore( codigo );
			}
		}

		if( codigo < pac.codigo ) {
			if( pac.posEsq != -1 ){
				posicionaNoRegistro( pac.posEsq );
				buscaArvore( codigo );
			}
		}

		if( codigo == pac.codigo ){
			return pac;
		}

	}
	pac.codigo = -1;

	return pac;
}

/** editaRegistro();
    Esta funcao recebe o n£mero do registro a ser alterado e retorna:
     1 - se o registro foi alterado
     0 - se o registro foi mantido inalterado
    -1 - se o nomero do registro  invalido
    -2 - se o registro est  excluido
*/
int editaRegistro( long nrRegistro ) {
	long recCount;
	paciente pac;

	fseek( arqDados, 0, SEEK_END );
	recCount = ftell( arqDados ) / sizeof( paciente );

	if( nrRegistro > 0 && nrRegistro < recCount ) {

	    posicionaNoRegistro( nrRegistro );
	    fread( &pac, sizeof( paciente ), 1, arqDados );

	    if( pac.codigo < 0 ) {
	    	return -2;
	    }

	    if( editaPaciente( &pac, -2 ) ) {
			posicionaNoRegistro( nrRegistro );
			fwrite( &pac, sizeof( paciente ), 1, arqDados );
			return 1;
	    } else {
	    	return 0;
	    }
	} else {
	    return -1;
	}
}

int editaRegistroArvore( long codigo ) {
	posicionaNoRegistro( 1 );

	paciente pac;

	pac = buscaArvore( codigo );

	if( pac.codigo < 0 ) {
		return -2;
	}

	if( editaPaciente( &pac, -2 ) ) {
		fseek( arqDados, - ( sizeof( paciente ) ), SEEK_CUR );
		fwrite( &pac, sizeof( paciente ), 1, arqDados );
		return 1;
	} else {
		return 0;
	}
}

int excluiRegistro( long codigo ) {

	long excluido = -1;

	fseek( arqDados, 0, SEEK_END );

	paciente pac = buscaArvore( codigo );
	fseek( arqDados, - ( sizeof( paciente ) ), SEEK_CUR );

	if( pac.codigo > 0 ){
		fwrite( &excluido, sizeof( long ), 1, arqDados );
		return 1;
	} else {
		return 0;
	}
}

void excluiPaciente() {
	long codigo;

	clrscr();
	gotoxy( 5, 3 );
	printf( "Informe o codigo do paciente a ser excluido: " );
	scanf( "%ld", &codigo );

	if( excluiRegistro( codigo ) ) {
	    printf( "\nRegistro foi excluido" );
	} else {
		printf( "\nnumero de codigo invalido: %ld", codigo );
	}

	getch();
}

void alteraPaciente() {
	long codigo;

	clrscr();
	gotoxy( 5, 3 );
	printf( "Informe o Codigo do paciente a ser alterado: " );

	scanf( "%ld", &codigo );

	switch( editaRegistroArvore( codigo ) ) {
	    case -1: printf( "\n\nnumero de codigo invalido: %ld", codigo ); break;
	    case  0: printf( "\n\nA alteracao foi cancelada!" ); break;
	    case  1: printf( "\n\nRegistro foi alterado!" ); break;
	}

	getch();
}

void insereNaArvore( paciente *novo, long posicaoAtual ){
	paciente pac;

	if ( fread( &pac, sizeof( paciente ), 1, arqDados ) == 1 ){

		if( novo->codigo > pac.codigo ) {
			if( pac.posDir != -1 ){
				posicionaNoRegistro( pac.posDir );
				insereNaArvore( novo, posicaoAtual );
			}else {
				pac.posDir = posicaoAtual;
				fseek( arqDados, - ( sizeof( paciente ) ), SEEK_CUR );
				fwrite( &pac, sizeof( paciente ), 1, arqDados );
			}
		}

		if( novo->codigo < pac.codigo ) {
			if( pac.posEsq != -1 ){
				posicionaNoRegistro( pac.posEsq );
				insereNaArvore( novo, posicaoAtual );
			} else {
				pac.posEsq = posicaoAtual;
				fseek( arqDados, - ( sizeof( paciente ) ), SEEK_CUR );
				fwrite( &pac, sizeof( paciente ), 1, arqDados );
			}
		}

	}
}


void inserePaciente() {
	paciente novo;
	long posicaoAtual;
	novo.codigo = 0;
	novo.nome[ 0 ] = 0;
	novo.telefone[ 0 ] = 0;
	novo.email[ 0 ] = 0;
	novo.peso = 0;
	novo.altura = 0;

	novo.posDir = -1;
	novo.posEsq = -1;

	if( editaPaciente( &novo, 0 ) ) {
		fseek( arqDados, 0, SEEK_END );
		fwrite( &novo, sizeof( paciente ), 1, arqDados );

		posicaoAtual = ftell( arqDados ) / sizeof( paciente );
		if( posicaoAtual > 1 ){
			fseek( arqDados, 0, SEEK_SET );
			insereNaArvore( &novo, posicaoAtual );
		}
	} else {
	    gotoxy( 10, 15 );
	    printf( "INCLUSAO FOI CANCELADA" );
	    getch();
	}
}

void abreArquivo() {

	clrscr();

	arqDados = fopen( "dadosarvore.dat", "rb+" );

	if( arqDados == 0 ) {

	    arqDados = fopen( "dadosarvore.dat", "wb+" );

	    if( arqDados == 0 ) {
		printf( "Nao posso prosseguir pois nao consigo abrir o arquivo \"dados.dat\"" );
		getch();
		exit( 0 );
	    }
	}
}


void listaArvore(){
	paciente pac;
	if( fread( &pac, sizeof( paciente ), 1, arqDados ) == 1 ){

		if( pac.posEsq != -1 ){
			posicionaNoRegistro( pac.posEsq );
			listaArvore();
		}

		if( pac.codigo > 0 ){
			printf( "\n  %.8ld     %-21.21s   %-10.10s     %-23.23s	   ", pac.codigo, pac.nome, pac.telefone, pac.email );
		}

		if( pac.posDir != -1 ){
			posicionaNoRegistro( pac.posDir );
			listaArvore();
		}
	}
}

void listaPacientes() {

	posicionaNoRegistro( 1 );
	clrscr();
	printf( "-[ Codigo ]-|-[ Nome ]--------------|-[ Telefone ]-|-[ eMail ]----------------\n" );

	listaArvore();
	getch();
}

void reoganizaArvore( paciente *pPac, long posicaoAtual, FILE *novo ){
	paciente pac;

	if ( fread( &pac, sizeof( paciente ), 1, novo ) == 1 ){

		if( pPac->codigo > pac.codigo ) {
			if( pac.posDir != -1 ){
				posicionaNoRegistro( pac.posDir );
				reoganizaArvore( pPac, posicaoAtual, novo );
			}else {
				pac.posDir = posicaoAtual;
				fseek( arqDados, - ( sizeof( paciente ) ), SEEK_CUR );
				fwrite( &pac, sizeof( paciente ), 1, novo );
			}
		}

		if( pPac->codigo < pac.codigo ) {
			if( pac.posEsq != -1 ){
				posicionaNoRegistro( pac.posEsq );
				reoganizaArvore( pPac, posicaoAtual, novo );
			} else {
				pac.posEsq = posicaoAtual;
				fseek( arqDados, - ( sizeof( paciente ) ), SEEK_CUR );
				fwrite( &pac, sizeof( paciente ), 1, novo );
			}
		}

	}
}

void reorganiza() {
	FILE *novo;
	paciente pac;
	long posicaoAtual = 1;
	novo = fopen( "ts73623.nov", "wb+" );
	fseek( novo, sizeof( paciente), SEEK_END );
	posicionaNoRegistro( posicaoAtual );
	while( fread( &pac, sizeof( paciente ), 1, arqDados ) == 1 ) {
		pac.posDir = -1;
		pac.posEsq = -1;
	    if( pac.codigo > 0 ) {
	    	fwrite( &pac, sizeof( paciente ), 1, novo );
	    	if( posicaoAtual > 1 ){
	    		reoganizaArvore( &pac, posicaoAtual, novo );
	    	}
	    	posicaoAtual ++;
	    }
	}

	fclose( novo );
	fclose( arqDados );

	remove( "dados.bak" );
	rename( "dados.dat", "dados.bak" );
	rename( "ts73623.nov", "dados.dat" );

	abreArquivo();
}

void reorganizaArquivo() {
	int opcao;

	clrscr();
	gotoxy( 5, 5 );
	printf( "Confirma a reorganizacao do arquivo (s - sim/N - nao)? " );
	opcao = getch();

	switch( opcao ) {
	    case 's':
	    case 'S': reorganiza();
		      printf( "\n\nO arquivo foi reorganizado" );
		      break;
	    default: printf( "\n\nOperacao cancelada!" );
	}

	getch();
}



int main(int argc, char **argv) {
	int opcao;

	abreArquivo();

	while( 1 ) {
		clrscr();
		printf( "1 - Inserir" );
		printf( "\n2 - Alterar" );
		printf( "\n3 - Excluir" );
		printf( "\n4 - Listar" );
		printf( "\n9 - Reorganizar" );
		printf( "\n" );
		printf( "\nX - Sair do Programa" );

		opcao = getch();
		printf( "\nopco %d", opcao );
		switch( opcao ) {
		    case '1': inserePaciente(); break;
		    case '2': alteraPaciente(); break;
		    case '3': excluiPaciente(); break;
		    case '4': listaPacientes(); break;
		    case '9': reorganizaArquivo(); break;
		    case 'x':
		    case 'X':
			clrscr();
			printf("\n\n\n Obrigado por utilizar nosso gerenciador\n\n\n");
			fclose( arqDados );
			exit( 0 );
		}
	}
}
