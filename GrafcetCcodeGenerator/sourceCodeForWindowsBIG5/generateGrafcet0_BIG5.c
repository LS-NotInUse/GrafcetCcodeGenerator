//
//  generateGrafcet0.c
//  generateGrafcetCcode
//
//  Created by LouisSung on 2017/11/1.
//  Copyright c 2017�~ LS. All rights reserved.
//
#include <string.h>
#include "generateGrafcet0_BIG5.h"
#define MAX_PATH_LENGTH (50)

static void getState(void) ;
static void genHeaderAndStateVariable(void), genMain(void), genGrafcet(void), genAction(void) ;
//***��X���A�Ρ�
static void genStateCheck(const char* _EOLorEMPTY) ;
//***��X���A�Ρ�

extern const char* EOL ;
static const char state[36+1]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
static FILE *fptC=NULL ;
static int howManyGrafcet=0 ;
static char subGrafcetList[36]={} ;
static int howManyStateOfSubGrafcet[36]={} ;

void generateGrafcet0(void){
	extern const char outputPath[] ;
	char cFile[MAX_PATH_LENGTH], hFile[MAX_PATH_LENGTH] ;
	sprintf(cFile, "%sgrafcet0.c", outputPath==NULL?"":outputPath) ;
	sprintf(hFile, "%sgrafcet0.h", outputPath==NULL?"":outputPath) ;
	
	//==============================
	getState() ;
	fptC=fopen(cFile, "w") ;
	genHeaderAndStateVariable() ;
	genMain() ;
	genGrafcet() ;
	genAction() ;
	//***��X���A�Ρ�
	genStateCheck("") ;
	//***��X���A�Ρ�
	fclose(fptC) ;
	//==============================
}

static void getState(){
	printf("G0���X�Ӫ��A�H(2~36)�G ") ;
	scanf("%d", &howManyGrafcet) ;
	printf("���Ǫ��A��subGrafcet�H(���A�������ݹj�})([1-9A-Z]+ | no)�G ") ;
	scanf("%s", subGrafcetList) ;
	if(subGrafcetList[0]=='n' && subGrafcetList[1]=='o'){
		subGrafcetList[0]='\0' ;
		subGrafcetList[1]='\0' ;
	}
}

static void genHeaderAndStateVariable(){
	//---���v�n��
	fprintf(fptC, "//_auto created by LS' GrafcetCcodeGenerator ?�E �s �E?%s", EOL) ;
	
	//---�ޤJ���Y��
	fprintf(fptC, "#include <stdio.h>%s", EOL) ;
	for(int i=0; i<strlen(subGrafcetList); ++i){			//_include"subGrafcets.h"
		fprintf(fptC, "#include \"grafcet%c.h\"%s", subGrafcetList[i], EOL) ;}
	
	//---�禡�ŧi
	fprintf(fptC, "%s", EOL) ;			//_grafcet0()�Baction0()���쫬�ŧi
	fprintf(fptC, "void grafcet0(void) ;%sstatic void action0(void) ;%s", EOL, EOL) ;
	
	//***��X���A�Ρ�
	fprintf(fptC, "//***��X���A�Ρ�%sstatic void stateCheck(void) ;%s//***��X���A�Ρ�%s", EOL, EOL, EOL) ;
	//***��X���A�Ρ�
	
	//---Grafcet0���A�ܼƫŧi
	fprintf(fptC, "%s", EOL) ;
	fprintf(fptC, "int x0=1, ") ;			//_G0���A�ܼƫŧi
	for(int i=1; i<howManyGrafcet; ++i){			//_G1~Glast���A�ܼƫŧi
		fprintf(fptC, "x%c=0%s", state[i], i==howManyGrafcet-1?" ;":", ") ;}
	fprintf(fptC, "%s", EOL) ;
	
	//---subGrafcet���A�ܼƫŧi
	if(strlen(subGrafcetList)!=0){
		printf("\n===\n") ;}
	for(int i=0; i<strlen(subGrafcetList); ++i){
		printf("G%c ��subGrafcet���X�Ӫ��A�H(2~36)�G ", subGrafcetList[i]) ;
		int indexOfSubGrafcet=subGrafcetList[i]<='9'?subGrafcetList[i]-'0':subGrafcetList[i]-'A'+10 ;			//_��subGrafcet��state�����s��
		scanf("%d", &howManyStateOfSubGrafcet[indexOfSubGrafcet]) ;			//_�������Ǧ�subGrafcet
		fprintf(fptC, "extern int ") ;
		for(int j=0; j<howManyStateOfSubGrafcet[indexOfSubGrafcet]; ++j){
			fprintf(fptC, "x%c%c%s", state[indexOfSubGrafcet], state[j], j==howManyStateOfSubGrafcet[indexOfSubGrafcet]-1?" ;":", ") ;}
		fprintf(fptC, "%s", EOL) ;
	}
	fseek(fptC, -(strlen(EOL)), SEEK_END) ;			//_�����̫᪺�ťզ�
}

static void genMain(){
	//---����main()
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC,"int main(){%s\twhile(1){%s\t\tgrafcet0() ;}%s}", EOL, EOL, EOL) ;
}

static void genGrafcet(){
	//_Gi=�{�b���A, Gj=�U�@���A, Gk=��L�ӧO���A, Gns=�W�U�h���A, []*t=[�ʧ@]��t��
	//---����grafcet()
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC, "void grafcet0(){%s", EOL) ;
	
	//---���ͪ��A�ಾ����
	enum translationType{SINGLE=1, D_AND, D_OR, C_AND, C_OR}previousType=0, currentType=0 ;
	const char tType[6][7]={"", "SINGLE", "D_AND", "D_OR", "C_AND", "C_OR"} ;			//`enum->string
	char translationTo[36]={} ;
	char cAndList[36][36]={} ;
	unsigned char remindConvergenceFlag=0 ;
	printf("\n===\n") ;
	for(int i=0; i<howManyGrafcet; ++i){
		//---C_AND:�h��Gn���Ħ�Gi
		if(strlen(cAndList[i])!=0){
			currentType=C_AND ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`���j���P�����϶�
			fprintf(fptC, "\t%s(", i==0?"if":"else if") ;			//_�Ĥ@��if, ��L��else if
			for(int j=0; j<strlen(cAndList[i]); ++j){				//_if(xi==1&&xns==1...)
				fprintf(fptC, "x%c==1%s", cAndList[i][j], j==strlen(cAndList[i])-1?"":"&&") ;
				int indexOfCAndList=(cAndList[i][j]<='9'?cAndList[i][j]-'0':cAndList[i][j]-'A'+10) ;
				if(howManyStateOfSubGrafcet[indexOfCAndList]!=0){			//_�U�hsubGrafcet������, �W�h�~�ಾ
					fprintf(fptC, "%sx%c%c==1%s", j==strlen(cAndList[i])-1?"&&":"", state[indexOfCAndList], state[howManyStateOfSubGrafcet[indexOfCAndList]-1], j==strlen(cAndList[i])-1?"":"&&") ;}
			}
			fprintf(fptC, "/*&& (|__C-AND__") ;
			printf("��C_AND�G") ;
			for(int j=0; j<strlen(cAndList[i]); ++j){
				fprintf(fptC, "G%c%s", cAndList[i][j], j==strlen(cAndList[i])-1?"":",") ;
				printf("G%c%s", cAndList[i][j], j==strlen(cAndList[i])-1?"":",") ;
			}
			fprintf(fptC, "��G%c��L����__|)*/){", state[i]) ;				//_Gns...->Gi
			printf("���Ĩ�G%c\n", state[i]) ;
			for(int j=0; j<strlen(cAndList[i]); ++j){			//_{xns=0...,xi=1}
				fprintf(fptC, "x%c=0; ", cAndList[i][j]) ;
				int indexOfCAndList=(cAndList[i][j]<='9'?cAndList[i][j]-'0':cAndList[i][j]-'A'+10) ;
				if(howManyStateOfSubGrafcet[indexOfCAndList]!=0){			//_�W�hGrafcet����U�h�ಾ
					fprintf(fptC, "x%c%c=0; x%c0=1; ", state[indexOfCAndList], state[howManyStateOfSubGrafcet[indexOfCAndList]-1], state[indexOfCAndList]) ;}
			}
			fprintf(fptC, "x%c=1 ;%s}%s", state[i], i==0?"/*G0��l�Ƥ��e*/":"", EOL) ;
		}
		
		//---�T�{Gi���A(SINGLE/D_AND/D_OR/C_AND/C_OR)
		printf("G%c�|�ಾ����Ǫ��A�H(���A�������ݹj�})(%s[0-9A-Z]+)�G ", state[i], remindConvergenceFlag==0?"":"(and|or)?") ;			//_���X�{D_AND��D_OR, �s�W�e��ﶵ�i�ѿ�J
		scanf("%s", translationTo) ;
		char divergenceType[4]={} ;			//_�ΨӰO���OD_AND��D_OR
		if(strlen(translationTo)>1){
			if(strncmp(translationTo, "and", 3)!=0 && strncmp(translationTo, "or", 2)!=0){			//_�Land/or�e��
				if(remindConvergenceFlag==0){			//_�Ĥ@���X�{D_AND��D_OR��, �����ϥΪ�
					printf("\n������ �Q�nConvergence�ɽЦb�ಾ���A�[�Wand/or�e��(ex:or5�|C_OR��G5)\n\n") ;
					remindConvergenceFlag=1 ;
				}
				printf("G%c Divergence��h�Ӫ��A�OD_AND�٬OD_OR�H(and|or)�G ", state[i]) ;
				scanf("%s", divergenceType) ;
			}}
		
		//---D_AND:Gi�o���ܦh��Gn
		if(strcmp(divergenceType, "and")==0){
			currentType=D_AND ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}		//`^
			fprintf(fptC, "\t%s", i==0?"if":"else if") ;		//_if(xi==1)
			fprintf(fptC, "(x%c==1", state[i]) ;
			if(howManyStateOfSubGrafcet[i]!=0){//^
				fprintf(fptC, "&&x%c%c==1", state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}
			fprintf(fptC, "/*&& (|__D-AND__G%c��", state[i]) ;
			for(int j=0; j<strlen(translationTo); ++j){		//_Gi->Gns...
				fprintf(fptC, "G%c%s", translationTo[j], j==strlen(translationTo)-1?"":",") ;}
			fprintf(fptC, "��L����__|)*/){x%c=0; ", state[i]) ;
			if(howManyStateOfSubGrafcet[i]!=0){			//^
				fprintf(fptC, "x%c%c=0; x%c0=1; ", state[i], state[howManyStateOfSubGrafcet[i]-1], state[i]) ;}
			for(int j=0; j<strlen(translationTo); ++j){			//_{xi=0, xns=1...}
				fprintf(fptC, "x%c=1%s", translationTo[j], j==strlen(translationTo)-1?" ;":"; ") ;}
			fprintf(fptC, "%s}%s", i==0?"/*G0��l�Ƥ��e*/":"", EOL) ;
		}
		
		//---D_OR:Gi�o���ܦh��Gns
		else if(strcmp(divergenceType, "or")==0){
			currentType=D_OR ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`^
			for(int j=0; j<strlen(translationTo); ++j){			//_if(xi==1)
				fprintf(fptC, "\t%s", i==0&&j==0?"if":"else if") ;			//_[Gi->Gj]*t
				fprintf(fptC, "(x%c==1", state[i]) ;
				if(howManyStateOfSubGrafcet[i]!=0){//^
					fprintf(fptC, "&&x%c%c==1", state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}
				fprintf(fptC, "/*&& (|__D-OR__G%c��G%c��L����__|)*/){x%c=0; ", state[i], translationTo[j], state[i]) ;
				if(howManyStateOfSubGrafcet[i]!=0){			//^
					fprintf(fptC, "x%c%c=0; x%c0=1; ", state[i], state[howManyStateOfSubGrafcet[i]-1], state[i]) ;}
				fprintf(fptC, "x%c=1 ;%s}%s", translationTo[j], i==0?"/*G0��l�Ƥ��e*/":"", EOL) ;			//_[{xi=0; xj=1}]*t
			}
		}
		
		//---C_OR:�h��Gns���Ħ�Gi
		else if(remindConvergenceFlag==1 && strncmp(translationTo, "or", 2)==0){			//_if(xj==1)
			currentType=C_OR ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`^
			fprintf(fptC, "\t%s", i==0?"if":"else if") ;			//_Gj->Gi
			fprintf(fptC, "(x%c==1", state[i]) ;
			if(howManyStateOfSubGrafcet[i]!=0){//^
				fprintf(fptC, "&&x%c%c==1", state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}
			fprintf(fptC, "/*&& (|__C-OR__G%c��G%c��L����__|)*/){x%c=0; ", state[i], translationTo[2], state[i]) ;
			if(howManyStateOfSubGrafcet[i]!=0){			//^
				fprintf(fptC, "x%c%c=0; x%c0=1; ", state[i], state[howManyStateOfSubGrafcet[i]-1], state[i]) ;}
			fprintf(fptC, "x%c=1 ;%s}%s", translationTo[2], i==0?"/*G0��l�Ƥ��e*/":"", EOL) ;			//_{xj=0, xi=1}
		}
		
		//---����C_AND:(�ȫت�)�h��Gns���Ħ�Gi
		else if(remindConvergenceFlag==1 && strncmp(translationTo, "and", 3)==0){
			int indexOfTranslation=translationTo[3]<='9'?translationTo[3]-'0':translationTo[3]-'A'+10 ;			//_���ಾ�ؼЪ�state�����s��
			cAndList[indexOfTranslation][strlen(cAndList[indexOfTranslation])]=state[i] ;			//_Gi���Ĩ�Gk, �����bGk����, �̫�Gk����|���������AGi,Gns...���Ĩ�L
		}
		
		//---SINGLE:���q����u�ಾGi->Gj
		else{			//_if(xi==1)
			currentType=SINGLE ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`^
			fprintf(fptC, "\t%s", i==0?"if":"else if") ;		//_Gi->Gj
			fprintf(fptC, "(x%c==1", state[i]) ;
			if(howManyStateOfSubGrafcet[i]!=0){//^
				fprintf(fptC, "&&x%c%c==1", state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}
			fprintf(fptC, "/*&& (|__SINGLE__G%c��G%c��L����__|)*/){x%c=0; ", state[i], translationTo[0], state[i]) ;
			if(howManyStateOfSubGrafcet[i]!=0){			//^
				fprintf(fptC, "x%c%c=0; x%c0=1; ", state[i], state[howManyStateOfSubGrafcet[i]-1], state[i]) ;}
			fprintf(fptC, "x%c=1 ;%s}%s", translationTo[0], i==0?"/*G0��l�Ƥ��e*/":"", EOL) ;			//_{xi=0, xj=1}
		}
		previousType=currentType ;
	}
	
	//---�S��C_AND:(Gns�^��G0(n>0))�h��Gns���Ħ�G0
	if(strlen(cAndList[0])!=0){//^			//_�����@��C_AND, listIndex=0
		currentType=C_AND ;
		if(currentType!=previousType){
			fprintf(fptC, "%s/**%s**/%s", EOL, tType[currentType], EOL) ;}			//`^
		fprintf(fptC, "\telse if(") ;
		for(int i=0; i<strlen(cAndList[0]); ++i){
			fprintf(fptC, "x%c==1%s", cAndList[0][i], i==strlen(cAndList[0])-1?"":"&&") ;
			int indexOfCAndList=(cAndList[0][i]<='9'?cAndList[0][i]-'0':cAndList[0][i]-'A'+10) ;
			if(howManyStateOfSubGrafcet[indexOfCAndList]!=0){			//^
				fprintf(fptC, "%sx%c%c==1%s", i==strlen(cAndList[0])-1?"&&":"", state[indexOfCAndList], state[howManyStateOfSubGrafcet[indexOfCAndList]-1], i==strlen(cAndList[0])-1?"":"&&") ;}
		}
		fprintf(fptC, "/*&& (|__C-AND__") ;
		printf("��C_AND�G") ;
		for(int i=0; i<strlen(cAndList[0]); ++i){
			fprintf(fptC, "G%c%s", cAndList[0][i], i==strlen(cAndList[0])-1?"":",") ;
			printf("G%c%s", cAndList[0][i], i==strlen(cAndList[0])-1?"":",") ;
		}
		fprintf(fptC, "��G0��L����__|)*/){") ;
		printf("���Ĩ�G0\n") ;
		for(int i=0; i<strlen(cAndList[0]); ++i){
			fprintf(fptC, "x%c=0; ", cAndList[0][i]) ;
			int indexOfCAndList=(cAndList[0][i]<='9'?cAndList[0][i]-'0':cAndList[0][i]-'A'+10) ;
			if(howManyStateOfSubGrafcet[indexOfCAndList]!=0){			//^
				fprintf(fptC, "x%c%c=0; x%c0=1; ", state[indexOfCAndList], state[howManyStateOfSubGrafcet[indexOfCAndList]-1], state[indexOfCAndList]) ;}
		}
		fprintf(fptC, "x0=1 ;}%s", EOL) ;
	}
	
	//***��X���A�Ρ�
	fprintf(fptC, "//***��X���A�Ρ�%s\tstateCheck() ;%s//***��X���A�Ρ�", EOL, EOL) ;
	//***��X���A�Ρ�
	
	//---����ActionFunctionCall
	fprintf(fptC, "%s\taction0() ;%s}", EOL, EOL) ;
}

static void genAction() {
	//---����action()
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC, "static void action0(){%s", EOL) ;
	for(int i=1; i<howManyGrafcet; ++i){
		fprintf(fptC, "\tif(x%c==1){", state[i]) ;
		if(howManyStateOfSubGrafcet[i]!=0){			//_�I�ssubGrafcet
			fprintf(fptC, "grafcet%c() ;", state[i]) ;}
		else{
			fprintf(fptC, "/*|__G%c�ʧ@__|*/", state[i]) ;}
		fprintf(fptC, "}%s", EOL) ;
	}
	fprintf(fptC, "}") ;
}

//***��X���A�Ρ�
static void genStateCheck(const char* EOLorEMPTY){
	//---����stateCheck()
	unsigned char eolFlag=!strcmp(EOLorEMPTY, EOL) ;			//_��������P�_
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC, "//***��X���A�Ρ�%sstatic void stateCheck(){%s\tprintf(\"��G0���A�G _\") ;%s", EOL, EOL, EOL) ;
	for(int i=0; i<howManyGrafcet; ++i){			//_G0�U���A
		if(eolFlag==1 || i==0){			//_(�Ĥ@��/������ɦUif����)���ť�
			fprintf(fptC, "\t") ;}
		fprintf(fptC, "if(x%c==1){printf(\"_x%c_\") ;}%s", state[i], state[i], EOLorEMPTY) ;
	}
	fprintf(fptC, "%s", EOL);			//_�C�@�դ�������
	for(int i=0; i<strlen(subGrafcetList); ++i){		//_subGrafcet�U���A
		if(eolFlag==0){
			fprintf(fptC, "\t") ;}		//_����ɪ��ť�
		int numOfSG=subGrafcetList[i]<='9'?subGrafcetList[i]-'0':subGrafcetList[i]-'A'+10 ;
		for(int j=0; j<howManyStateOfSubGrafcet[numOfSG]; ++j){
			if(eolFlag==1){
				fprintf(fptC, "\t") ;}
			fprintf(fptC, "if(x%c%c==1){printf(\"_|x%c%c|_\") ;}%s", subGrafcetList[i], state[j], subGrafcetList[i], state[j], EOLorEMPTY) ;
		}
		fprintf(fptC, "%s", EOL) ;
	}
	if(eolFlag==1){
		fseek(fptC, -(strlen(EOL)), SEEK_END) ;}		//_�����h�l���ťզ�
	fprintf(fptC, "\tprintf(\"_\\n\") ;%s}%s//***��X���A�Ρ�", EOL, EOL) ;
}
//***��X���A�Ρ�
