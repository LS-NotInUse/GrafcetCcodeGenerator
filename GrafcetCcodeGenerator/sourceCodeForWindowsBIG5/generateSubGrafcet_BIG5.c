//
//  generateSubGrafcet.c
//  generateGrafcetCcode
//
//  Created by LouisSung on 2017/11/3.
//  Copyright c 2017�~ LS. All rights reserved.
//
#include <string.h>
#include "generateSubGrafcet_BIG5.h"
#define MAX_PATH_LENGTH (50)

static void getState(void) ;
static void genHeaderAndStateVariable(void), genGrafcet(void), genAction(void) ;//$
static void genHeaderFile(void) ;//+
//***��X���A�Ρ�
static void genStateCheck(const char* _EOLorEMPTY) ;
//***��X���A�Ρ�

extern const char* EOL ;
extern char grafcetID[6] ;//+
static const char state[36+1]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
static FILE *fptC=NULL ;
static FILE *fptH=NULL ;//+
static int howManyGrafcet=0 ;
static char subGrafcetList[36]={} ;
static int howManyStateOfSubGrafcet[36]={} ;

void generateSubGrafcet(void){//$
	extern const char outputPath[] ;
	char cFile[MAX_PATH_LENGTH], hFile[MAX_PATH_LENGTH] ;
	sprintf(cFile, "%sgrafcet%s.c", outputPath==NULL?"":outputPath, grafcetID) ;//$
	sprintf(hFile, "%sgrafcet%s.h", outputPath==NULL?"":outputPath, grafcetID) ;//$
	
	//==============================
	getState() ;
	fptC=fopen(cFile, "w") ;
	genHeaderAndStateVariable() ;
	genGrafcet() ;
	genAction() ;
	//***��X���A�Ρ�
	genStateCheck("") ;
	//***��X���A�Ρ�
	fclose(fptC) ;
	//==============================
	fptH=fopen(hFile, "w") ;//+
	genHeaderFile() ;//+
	fclose(fptH) ;//+
	//==============================
}

static void getState(){
	printf("G%s���X�Ӫ��A�H(2~36)�G ", grafcetID) ;//$
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
	fprintf(fptC, "#include \"grafcet%s.h\"%s", grafcetID, EOL) ;//$
	for(int i=0; i<strlen(subGrafcetList); ++i){			//_include"subSubGrafcets.h"
		fprintf(fptC, "#include \"grafcet%s%c.h\"%s", grafcetID, subGrafcetList[i], EOL) ;}//$
	
	//---�禡�ŧi
	fprintf(fptC, "%s", EOL) ;			//_grafcet#()�Baction#()���쫬�ŧi
	fprintf(fptC, "static void action%s(void) ;%s", grafcetID, EOL) ;//$
	
	//***��X���A�Ρ�
	fprintf(fptC, "//***��X���A�Ρ�%sstatic void stateCheck(void) ;%s//***��X���A�Ρ�%s", EOL, EOL, EOL) ;
	//***��X���A�Ρ�
	
	//---Grafcet#���A�ܼƫŧi
	fprintf(fptC, "%s", EOL) ;
	fprintf(fptC, "extern int x%s ;%s", grafcetID, EOL) ;//+			//_�ޤJ�ۤv�����A�ܼ�
	fprintf(fptC, "int x%s0=1, ", grafcetID) ;//$			//_G#0���A�ܼƫŧi
	for(int i=1; i<howManyGrafcet; ++i){			//_G#1~G#last���A�ܼƫŧi
		fprintf(fptC, "x%s%c=0%s", grafcetID, state[i], i==howManyGrafcet-1?" ;":", ") ;}//$
	fprintf(fptC, "%s", EOL) ;
	
	//---subSubGrafcet���A�ܼƫŧi
	if(strlen(subGrafcetList)!=0){
		printf("\n===\n") ;}
	for(int i=0; i<strlen(subGrafcetList); ++i){
		printf("G%s%c��subGrafcet���X�Ӫ��A�H(2~36)�G ", grafcetID, subGrafcetList[i]) ;//$
		int indexOfSubGrafcet=0 ;
		if('0'<=subGrafcetList[i] && subGrafcetList[i]<='9'){			//_��subSubGrafcet��state�����s��
			indexOfSubGrafcet=subGrafcetList[i]-'0' ;}			//_'0'->0
		else if('A'<=subGrafcetList[i] && subGrafcetList[i]<='Z'){
			indexOfSubGrafcet=subGrafcetList[i]-'A'+10 ;}		//_'A'->10
		scanf("%d", &howManyStateOfSubGrafcet[indexOfSubGrafcet]) ;			//_�������Ǧ�subSubGrafcet
		fprintf(fptC, "extern int ") ;
		for(int j=0; j<howManyStateOfSubGrafcet[indexOfSubGrafcet]; ++j){
			fprintf(fptC, "x%s%c%c%s", grafcetID, state[indexOfSubGrafcet], state[j], j==howManyStateOfSubGrafcet[indexOfSubGrafcet]-1?" ;":", ") ;}//$
		fprintf(fptC, "%s", EOL) ;
	}
	fseek(fptC, -(strlen(EOL)), SEEK_END) ;			//_�����̫᪺�ťզ�
}

static void genGrafcet(){
	//_Gi=�{�b���A, Gj=�U�@���A, Gk=��L�ӧO���A, Gns=�W�U�h���A, []*t=[�ʧ@]��t��
	//---����grafcet()
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC, "void grafcet%s(){%s", grafcetID, EOL) ;//$
	
	//---���ͪ��A�ಾ����
	enum translationType{SINGLE=1, D_AND, D_OR, C_AND, C_OR}previousType=0, currentType=0 ;
	const char tType[6][7]={"","SINGLE", "D_AND", "D_OR", "C_AND", "C_OR"} ;			//`enum->string
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
				fprintf(fptC, "x%s%c==1%s", grafcetID, cAndList[i][j], j==strlen(cAndList[i])-1?"":"&&") ;//$
				int indexOfCAndList=(cAndList[i][j]<='9'?cAndList[i][j]-'0':cAndList[i][j]-'A'+10) ;
				if(howManyStateOfSubGrafcet[indexOfCAndList]!=0){			//_�U�hsubGrafcet������, �W�h�~�ಾ
					fprintf(fptC, "%sx%s%c%c==1%s", j==strlen(cAndList[i])-1?"&&":"", grafcetID,  state[indexOfCAndList], state[howManyStateOfSubGrafcet[indexOfCAndList]-1], j==strlen(cAndList[i])-1?"":"&&") ;}
			}
			fprintf(fptC, "/*&& (|__C-AND__") ;
			printf("C_AND�G") ;
			for(int j=0; j<strlen(cAndList[i]); ++j){
				fprintf(fptC, "G%s%c%s", grafcetID, cAndList[i][j], j==strlen(cAndList[i])-1?"":",") ;//$
				printf("G%s%c%s", grafcetID, cAndList[i][j], j==strlen(cAndList[i])-1?"":",") ;//$
			}
			fprintf(fptC, "��G%s%c��L����__|)*/){", grafcetID, state[i]) ;//$				//_Gns...->Gi
			printf("���Ĩ�G%s%c\n", grafcetID, state[i]) ;//$
			for(int j=0; j<strlen(cAndList[i]); ++j){			//_{xns=0...,xi=1}
				fprintf(fptC, "x%s%c=0; ", grafcetID, cAndList[i][j]) ;//$
				int indexOfCAndList=(cAndList[i][j]<='9'?cAndList[i][j]-'0':cAndList[i][j]-'A'+10) ;
				if(howManyStateOfSubGrafcet[indexOfCAndList]!=0){			//_�W�hGrafcet����U�h�ಾ
					fprintf(fptC, "x%s%c%c=0; x%s%c0=1; ", grafcetID, state[indexOfCAndList], state[howManyStateOfSubGrafcet[indexOfCAndList]-1], grafcetID, state[indexOfCAndList]) ;}//$
			}
			fprintf(fptC, "x%s%c=1 ;%s%s%s}%s", grafcetID, state[i], i==0?"/*G":"", i==0?grafcetID:"", i==0?"��l�Ƥ��e*/":"", EOL) ;//$
		}
		
		//---�T�{Gi���A(SINGLE/D_AND/D_OR/C_AND/C_OR)
		printf("G%s%c�|�ಾ����Ǫ��A�H(���A�������ݹj�})(%s[0-9A-Z]+)�G ", grafcetID, state[i], remindConvergenceFlag==0?"":"(and|or)?") ;//$			//_���X�{D_AND��D_OR, �s�W�e��ﶵ�i�ѿ�J
		scanf("%s", translationTo) ;
		char divergenceType[4]={} ;			//_�ΨӰO���OD_AND��D_OR
		if(strlen(translationTo)>1){
			if(strncmp(translationTo, "and", 3)!=0 && strncmp(translationTo, "or", 2)!=0){			//_�Land/or�e��
				if(remindConvergenceFlag==0){			//_�Ĥ@���X�{D_AND��D_OR��, �����ϥΪ�
					printf("\n������ �Q�nConvergence�ɽЦb�ಾ���A�[�Wand/or�e��(ex:or5�|C_OR��G5)\n\n") ;
					remindConvergenceFlag=1 ;
				}
				printf("G%s%c Divergence��h�Ӫ��A�OD_AND�٬OD_OR�H(and|or)�G ", grafcetID, state[i]) ;
				scanf("%s", divergenceType) ;
			}}
		
		//---�̫�@�Ӫ��A���ѤW�h������ಾ�ܪ�l���A(Gxlast��Gx0)//+
		if(i==howManyGrafcet-1 && translationTo[0]=='0'){//+
			if(strlen(translationTo)==1){
				break ;}			//���ಾ��Gx0
			else{
				memmove(translationTo, translationTo+1, strlen(translationTo));}}			//_�����ಾ��Gx0
		else if(i==howManyGrafcet-1 && strcmp(translationTo, "or0")==0){//+
			break ;}			//_�P�_�O�_��or0
		
		//---D_AND:Gi�o���ܦh��Gn
		if(strcmp(divergenceType, "and")==0){
			currentType=D_AND ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}		//`^
			fprintf(fptC, "\t%s", i==0?"if":"else if") ;		//_if(xi==1)
			fprintf(fptC, "(x%s%c==1%s%s%s", grafcetID, state[i], i==0?"&&x":"", i==0?grafcetID:"", i==0?"==1":"") ;//$
			if(howManyStateOfSubGrafcet[i]!=0){//^
				fprintf(fptC, "&&x%s%c%c==1",grafcetID , state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}//$
			fprintf(fptC, "/*&& (|__D-AND__G%s%c��", grafcetID, state[i]) ;//$
			for(int j=0; j<strlen(translationTo); ++j){		//_Gi->Gns...
				fprintf(fptC, "G%s%c%s", grafcetID, translationTo[j], j==strlen(translationTo)-1?"":",") ;}//$
			fprintf(fptC, "��L����__|)*/){x%s%c=0; ", grafcetID, state[i]) ;//$
			if(howManyStateOfSubGrafcet[i]!=0){			//^
				fprintf(fptC, "x%s%c%c=0; x%s%c0=1; ", grafcetID, state[i], state[howManyStateOfSubGrafcet[i]-1], grafcetID, state[i]) ;}//$
			for(int j=0; j<strlen(translationTo); ++j){			//_{xi=0, xns=1...}
				fprintf(fptC, "x%s%c=1%s", grafcetID, translationTo[j], j==strlen(translationTo)-1?" ;":"; ") ;}//$
			fprintf(fptC, "%s%s%s}%s", i==0?"/*G":"", i==0?grafcetID:"", i==0?"��l�Ƥ��e*/":"", EOL) ;
		}
		
		//---D_OR:Gi�o���ܦh��Gns
		else if(strcmp(divergenceType, "or")==0){
			currentType=D_OR ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`^
			for(int j=0; j<strlen(translationTo); ++j){			//_if(xi==1)
				fprintf(fptC, "\t%s", i==0&&j==0?"if":"else if") ;			//_[Gi->Gj]*t
				fprintf(fptC, "(x%s%c==1%s%s%s", grafcetID, state[i], i==0?"&&x":"", i==0?grafcetID:"", i==0?"==1":"") ;//$
				if(howManyStateOfSubGrafcet[i]!=0){//^
					fprintf(fptC, "&&x%s%c%c==1",grafcetID , state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}//$
				fprintf(fptC, "/*&& (|__D-OR__G%s%c��G%s%c��L����__|)*/){x%s%c=0; ", grafcetID, state[i], grafcetID, translationTo[j], grafcetID, state[i]) ;//$
				if(howManyStateOfSubGrafcet[i]!=0){			//^
					fprintf(fptC, "x%s%c%c=0; x%s%c0=1; ", grafcetID, state[i], state[howManyStateOfSubGrafcet[i]-1], grafcetID, state[i]) ;}//$
				fprintf(fptC, "x%s%c=1 ;%s%s%s}%s", grafcetID, translationTo[j], i==0?"/*G":"", i==0?grafcetID:"", i==0?"��l�Ƥ��e*/":"", EOL) ;//$			//_[{xi=0; xj=1}]*t
			}
		}
		
		//---C_OR:�h��Gns���Ħ�Gi
		else if(remindConvergenceFlag==1 && strncmp(translationTo, "or", 2)==0){			//_if(xj==1)
			currentType=C_OR ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`^
			fprintf(fptC, "\t%s", i==0?"if":"else if") ;			//_Gj->Gi
			fprintf(fptC, "(x%s%c==1", grafcetID, state[i]) ;//$
			if(howManyStateOfSubGrafcet[i]!=0){//^
				fprintf(fptC, "&&x%s%c%c==1",grafcetID , state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}//$
			fprintf(fptC, "/*&& (|__C-OR__G%s%c��G%s%c��L����__|)*/){x%s%c=0; ", grafcetID, state[i], grafcetID, translationTo[2], grafcetID, state[i]) ;//$
			if(howManyStateOfSubGrafcet[i]!=0){			//^
				fprintf(fptC, "x%s%c%c=0; x%s%c0=1; ", grafcetID, state[i], state[howManyStateOfSubGrafcet[i]-1], grafcetID, state[i]) ;}//$
			fprintf(fptC, "x%s%c=1 ;%s%s%s}%s", grafcetID, translationTo[2], i==0?"/*G":"", i==0?grafcetID:"", i==0?"��l�Ƥ��e*/":"", EOL) ;//$			//_{xj=0, xi=1}
		}
		
		//---����C_AND:(�ȫت�)�h��Gns���Ħ�Gi
		else if(remindConvergenceFlag==1 && strncmp(translationTo, "and", 3)==0){
			int indexOfTranslation=0 ;
			if(translationTo[3]<='9'){			//_���ಾ�ؼЪ�state�����s��
				indexOfTranslation=translationTo[3]-'0' ;}
			else if(translationTo[3]<='Z'){
				indexOfTranslation=translationTo[3]-'A'+10 ;}
			cAndList[indexOfTranslation][strlen(cAndList[indexOfTranslation])]=state[i] ;			//_Gi���Ĩ�Gk, �����bGk����, �̫�Gk����|���������AGi,Gns...���Ĩ�L
		}
		
		//---SINGLE:���q����u�ಾGi->Gj
		else{			//_if(xi==1)
			currentType=SINGLE ;
			if(currentType!=previousType){
				fprintf(fptC, "%s/**%s**/%s", i==0?"":EOL, tType[currentType], EOL) ;}			//`^
			fprintf(fptC, "\t%s", i==0?"if":"else if") ;		//_Gi->Gj
			fprintf(fptC, "(x%s%c==1%s%s%s", grafcetID, state[i], i==0?"&&x":"", i==0?grafcetID:"", i==0?"==1":"") ;//$
			if(howManyStateOfSubGrafcet[i]!=0){//^
				fprintf(fptC, "&&x%s%c%c==1",grafcetID , state[i], state[howManyStateOfSubGrafcet[i]-1]) ;}//$
			fprintf(fptC, "/*&& (|__SINGLE__G%s%c��G%s%c��L����__|)*/){x%s%c=0; ", grafcetID, state[i], grafcetID, translationTo[0], grafcetID, state[i]) ;//$
			if(howManyStateOfSubGrafcet[i]!=0){			//^
				fprintf(fptC, "x%s%c%c=0; x%s%c0=1; ", grafcetID, state[i], state[howManyStateOfSubGrafcet[i]-1], grafcetID, state[i]) ;}//$
			fprintf(fptC, "x%s%c=1 ;%s%s%s}%s", grafcetID, translationTo[0], i==0?"/*G":"", i==0?grafcetID:"", i==0?"��l�Ƥ��e*/":"", EOL) ;//$			//_{xi=0, xj=1}
		}
		previousType=currentType ;
	}
	
	//-�����S��C_AND:Gxns��Gx0//-
	
	//***��X���A�Ρ�
	fprintf(fptC, "//***��X���A�Ρ�%s\tstateCheck() ;%s//***��X���A�Ρ�", EOL, EOL) ;
	//***��X���A�Ρ�
	
	//---����ActionFunctionCall
	fprintf(fptC, "%s\taction%s() ;%s}", EOL, grafcetID, EOL) ;//$
}

static void genAction() {
	//---����action()
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC, "static void action%s(){%s", grafcetID, EOL) ;//$
	for(int i=1; i<howManyGrafcet; ++i){
		fprintf(fptC, "\tif(x%s%c==1){", grafcetID, state[i]) ;
		if(howManyStateOfSubGrafcet[i]!=0){			//_�I�ssubGrafcet
			fprintf(fptC, "grafcet%s%c() ;", grafcetID, state[i]) ;}
		else{
			fprintf(fptC, "/*|__G%s%c�ʧ@__|*/", grafcetID, state[i]) ;}
		fprintf(fptC, "}%s", EOL) ;
	}
	fprintf(fptC, "}") ;
}

//***��X���A�Ρ�
static void genStateCheck(const char* EOLorEMPTY){
	//---����stateCheck()
	unsigned char eolFlag=!strcmp(EOLorEMPTY, EOL) ;			//_��������P�_
	fprintf(fptC, "%s%s", EOL, EOL) ;
	fprintf(fptC, "//***��X���A�Ρ�%sstatic void stateCheck(){%s\tprintf(\"��G%s���A�G _\") ;%s", EOL, EOL, grafcetID, EOL) ;//$
	for(int i=0; i<howManyGrafcet; ++i){			//_G0�U���A
		if(eolFlag==1 || i==0){			//_(�Ĥ@��/������ɦUif����)���ť�
			fprintf(fptC, "\t") ;}
		fprintf(fptC, "if(x%s%c==1){printf(\"_", grafcetID, state[i]) ;//$
		for(int j=0; j<strlen(grafcetID); ++j){//+
			fprintf(fptC,"|") ;}
		fprintf(fptC, "x%s%c", grafcetID, state[i]) ;//$
		for(int j=0; j<strlen(grafcetID); ++j){//+
			fprintf(fptC,"|") ;}
		fprintf(fptC, "_\") ;}%s", EOLorEMPTY) ;//$
	}
	fprintf(fptC, "%s", EOL);			//_�C�@�դ�������
	for(int i=0; i<strlen(subGrafcetList); ++i){		//_subGrafcet�U���A
		if(eolFlag==0){
			fprintf(fptC, "\t") ;}		//_����ɪ��ť�
		int numOfSG=subGrafcetList[i]<='9'?subGrafcetList[i]-'0':subGrafcetList[i]-'A'+10 ;
		for(int j=0; j<howManyStateOfSubGrafcet[numOfSG]; ++j){
			if(eolFlag==1){
				fprintf(fptC, "\t") ;}
			fprintf(fptC, "if(x%s%c%c==1){printf(\"_", grafcetID, subGrafcetList[i], state[j]) ;//$
			for(int j=0; j<strlen(grafcetID)+1; ++j){//+
				fprintf(fptC,"|") ;}
			fprintf(fptC, "x%s%c%c_", grafcetID, subGrafcetList[i], state[j]) ;//$
			for(int j=0; j<strlen(grafcetID)+1; ++j){//+
				fprintf(fptC,"|") ;}
			fprintf(fptC,"\") ;}%s", EOLorEMPTY) ;//$
		}
		fprintf(fptC, "%s", EOL) ;
	}
	if(eolFlag==1){
		fseek(fptC, -(strlen(EOL)), SEEK_END) ;}		//_�����h�l���ťզ�
	fprintf(fptC, "\tprintf(\"_\\n\") ;%s}%s//***��X���A�Ρ�", EOL, EOL) ;
}
//***��X���A�Ρ�

void genHeaderFile(void){//+
	fprintf(fptH, "#ifndef grafcet%s_h%s#define grafcet%s_h%s%s#include <stdio.h>%svoid grafcet%s(void) ;%s%s#endif /* grafcet%s_h */", grafcetID, EOL, grafcetID, EOL, EOL, EOL, grafcetID, EOL, EOL, grafcetID) ;//+
}
