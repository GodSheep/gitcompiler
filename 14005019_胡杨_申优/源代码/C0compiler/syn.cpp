#include <cstdlib>
#include <string.h>

#include <iostream>
#include <fstream>

#include "global.h"

#include <vector>


#define CONST       41   //����
#define VARIABLE    42   //����
#define FUNCTION    43   //����
#define PARAINT     44   //����
#define PARACHAR    45



using namespace std;

int  value;     //���泣����ֵ�������ӳ���0��ʾvoid��1��ʾint
int  address;   //�����ĵ�ַ����������ֵ�ĵ�ַ
int  paranum;   //ȫ�ֱ������������������壬��Ϊ�����Ĳ�������ֻ���ڲ������ӳ���ִ�н���֮���֪��������ֻ���ں���Ų�����ű���Ϣ
int  kind;      //���ڴ���ı�־��������
int tpkind[MAXPARANUM];

int returnnum = 0;
int termtk=0;
char exprevalue[ 200 ];
char funcname[100];

int globdefine = 1;


char name [MAXIDENLEN]; //��־�������֣��������������ͺ���



typedef struct{
    char name[ MAXIDENLEN ];  //identifier name
    int kind;               //��ʶ�����ͣ�INTTK,CHARTK,CINTET, CCHARET,FUNCTION,PARA
    int value;              //���ں�����˵��2��ʾ����ֵΪchar��1��ʾ����ֵΪInt 0����ֵΪvoid
    int address;            //address��Ԫ���ڷ��ű�element�����е��±�
    int paranum;            //�Ժ�����˵�Ĳ�������

    int parakind[MAXPARANUM];
}symin;

typedef struct {
    symin element[ MAXSYMTABLENUM ];
    int index;
    int ftotal;                 //�ֳ�������
    int findextable[ MAXSYMTABLENUM ];//�ֳ�����������
}symtable;


symtable maintable;

//�����ű���
void pushtable(char* name, int kind, int value, int address, int paranum, int isVec = 0){

    //whether table is full
    if ( maintable.index >= MAXSYMTABLENUM ){
        error(OUTOFTABLE);
       // cout<<"FULLSYMBOLTABLE"<<endl;
        exit(1);
    }

    //whether conflict
    //function
    if ( kind == FUNCTION ) {
        int iter = 1;
        for ( iter = 1; iter <= maintable.ftotal; iter++ ) {
            if ( strcmp(maintable.element[ maintable.findextable[ iter ] ].name, name)==0 ) {
                error(SYMBOLCONFLICT);
                //cout<<"IDENTIFIERCONFLICT"<<endl;
                return;
            }
        }
        maintable.findextable[ ++maintable.ftotal ] = maintable.index;
    }
    else{
        //others�Ǻ���   �ں����ڲ����Ƿ������ֲ�����
        int iter = maintable.findextable[ maintable.ftotal ];
        for ( ; iter < maintable.index; iter++ ) {
            if ( strcmp(maintable.element[ iter ].name, name)==0 ) {
                error(SYMBOLCONFLICT);
              //  cout<<"IDENTIFIERCONFLICT"<<endl;
                return;
            }
        }
        //ȫ�ֱ�����Χ������ȫ�ֱ����Ƿ�����
        if ( globdefine ) {
            iter = 0;
            while ( iter < maintable.findextable[ 1 ] ) {
                if ( strcmp(maintable.element[ iter ].name, name) ==0 ) {
                    error(SYMBOLCONFLICT);
                    //cout<<"IDENTIFIERCONFLICT"<<endl;
                    return;
                }
                iter++;
            }
        }
    }

    strcpy(maintable.element[ maintable.index ].name, name);
    maintable.element[ maintable.index ].kind = kind;
    maintable.element[ maintable.index ].value = value;
    maintable.element[ maintable.index ].address = maintable.index;
    maintable.element[ maintable.index ].paranum = isVec ? isVec : paranum;
   /* if (maintable.index <= hiscnt) {
        symtablehis << "----------------------------------------------------------------" << endl;
    }
    hiscnt = maintable.index;*/
    //symtablehis << maintable.index << "\t" << name << "\t" << kind << "\t" << value << "\t" << address << "\t" << paranum << "\t" << isVec << endl;
    maintable.index++;
}


void updatepara(int paranum,int* tkind)
{
    int x = maintable.findextable[ maintable.ftotal ];   //array[i]��ָ����ǵ�ǰ���ڱ������ĺ����ڷ��ű��е�����
    maintable.element[ x ].paranum = paranum;//���뺯������������Ϣ

    for(int i=0;i<paranum;i++){
        maintable.element[x].parakind[i]=tkind[i];
    }

    return;
}


int arrnum(char *name){
    int n;
    int m = maintable.findextable[ 1 ];
    n = maintable.findextable[ maintable.ftotal ];
    //���ҷ��ű����Ƿ���
    while ( n < maintable.index ) {
        if ( strcmp(maintable.element[ n ].name, name) == 0 )
            return maintable.element[ n ].paranum;
        n++;
    }

    //����ֳ�����ű����޶��壬�����������ű�
    if ( n == maintable.index ) {
        n = 0;
        while ( n < m ) {
            if ( strcmp(maintable.element[ n ].name, name) == 0 )
                return maintable.element[ n ].paranum;
            n++;
        }
        //���ű���û�ж���
        if ( n == m ) {
            printf("\"%s\" ", name);
            //error(VARNOTDEFINE);
            return -1;
        }
    }
}

//����ű�
//flag=1ʱ��˵��Ҫ�ڷֳ�����������ң�flag=0ʱ���ڵ�ǰ���ڷֳ�����ű�������ȫ�ֱ����в���

int searchst(char* name0, int flag){
    int n;
    int m = maintable.findextable[ 1 ];
    if ( flag == 1 ) {

        n = 1;
        while ( n <= maintable.ftotal ) {
            if ( strcmp(maintable.element[ maintable.findextable[ n ] ].name, name0) == 0 )
                break;
            n++;
        }

        //���������־��û�ж���
        if ( n > maintable.ftotal ) {
            //printf("\"%s\" ", name);
            error(FUNCTIONNOTFOUND);//IWFLIRSPE
            cout<<"FUNCTIONNOTFOUND"<<endl;
         //   return -1 * FUNCTIONNOTFOUND;
            return -1;
        }

        //���������ʵ�θ������βθ�����ƥ��
        if ( maintable.element[ maintable.findextable[ n ] ].paranum != paranum ) {
            //printf("\"%s\" ", name);
            error(FORMALPARANUMUNMATCH);//DONOTHING
            cout<<"FUNCPARANUMNMATCH"<<endl;
          //  return -1 * FORMALPARANUMUNMATCH;
          return -1;
        }

        for(int j=0;j<paranum;j++){
            if(maintable.element[maintable.findextable[ n ]].parakind[j]!=tpkind[j]){
                cout<<"WARNING:PARATYPENOTMATCH"<<endl;
                error(PARATYPENOTMATCH);
              //  return -1;
            }
        }


        return maintable.element[ maintable.findextable[ n ] ].address;
    }
    else{
        n = maintable.findextable[ maintable.ftotal ];


        //���ҷ��ű����Ƿ���
        while ( n < maintable.index ) {
            if ( strcmp(maintable.element[ n ].name, name0) == 0 )
                break;
            n++;
        }

        //����ֳ�����ű����޶��壬�����������ű�
        if ( n == maintable.index ) {
            n = 0;
            while ( n < m ) {
                if ( strcmp(maintable.element[ n ].name, name0) == 0 )
                    break;
                n++;
            }
            //���ű���û�ж���
            if ( n == m ) {
                //printf("\"%s\" ", name);
                error(VARNOTDEFINE);//DONOTHING
                cout<<"VARNOTDEFINE"<<endl;
                return -1;
            }
        }

        if ( maintable.element[ n ].kind == INTTK || maintable.element[ n ].kind == CHARTK )
            return maintable.element[ n ].address;
        if ( maintable.element[ n ].kind == CINTET || maintable.element[ n ].kind == CCHARET )
            return maintable.element[ n ].address;    //const ���ظ�������ַ
        if ( maintable.element[ n ].kind == PARAINT ||  maintable.element[ n ].kind == PARACHAR )
            return maintable.element[ n ].address;    //������־
        return -1;
    }
}


//����ű���kind
//flag=1ʱ��˵��Ҫ�ڷֳ�����������ң�flag=0ʱ���ڵ�ǰ���ڷֳ�����ű�������ȫ�ֱ����в���

int searchkind(char* name0, int flag){
    int n;
    int m = maintable.findextable[ 1 ];
    if ( flag == 1 ) {
        n = 1;
        while ( n <= maintable.ftotal ) {
            if ( strcmp(maintable.element[ maintable.findextable[ n ] ].name, name0) == 0 )
                break;
            n++;
        }

        //���������־��û�ж���
        if ( n > maintable.ftotal ) {
            //printf("\"%s\" ", name);
    //        error(FUNCTIONNOTFOUND);
            cout<<"FUNCTIONNOTFOUND"<<endl;
         //   return -1 * FUNCTIONNOTFOUND;
            return -1;
        }

        //���������ʵ�θ������βθ�����ƥ��
        if ( maintable.element[ maintable.findextable[ n ] ].paranum != paranum ) {
            //printf("\"%s\" ", name);
   //         error(FORMALPARANUMUNMATCH);
            cout<<"FUNCPARANUMNMATCH"<<endl;
          //  return -1 * FORMALPARANUMUNMATCH;
          return -1;
        }
/*
        //���������ʶ��������
        if ( maintable.element[ maintable.findextable[ n ] ].value == 0 )
            //return -1 * FUNCTIONRETURNNULL;
            return -1;
*/

        return maintable.element[ maintable.findextable[ n ] ].value;
    }
    else{
        n = maintable.findextable[ maintable.ftotal ];


        //���ҷ��ű����Ƿ���
        while ( n < maintable.index ) {
            if ( strcmp(maintable.element[ n ].name, name0) == 0 )
                break;
            n++;
        }

        //����ֳ�����ű����޶��壬�����������ű�
        if ( n == maintable.index ) {
            n = 0;
            while ( n < m ) {
                if ( strcmp(maintable.element[ n ].name, name0) == 0 )
                    break;
                n++;
            }
            //���ű���û�ж���
            if ( n == m ) {
                //printf("\"%s\" ", name);
    //            error(VARNOTDEFINE);
                cout<<"VARNOTDEFINE"<<endl;
                return -1;
            }
        }



        if ( maintable.element[ n ].kind == INTTK || maintable.element[ n ].kind == CINTET || maintable.element[ n ].kind == PARAINT )
            return INTTK;
        if ( maintable.element[ n ].kind == CHARTK || maintable.element[ n ].kind == CCHARET || maintable.element[ n ].kind == PARACHAR ){
           // cout<<  CHARTK << "     33333333333333" << endl;
            return CHARTK;    //const ���ظ�������ַ
        }


        return -1;
    }
}





void popst(){
    int x = maintable.index - 1;

    while ( ( maintable.element[ x ].kind == INTTK || maintable.element[ x ].kind == CHARTK || maintable.element[ x ].kind == CINTET
            || maintable.element[ x ].kind == CCHARET || maintable.element[ x ].kind == PARAINT || maintable.element[ x ].kind == PARACHAR)
            && strcmp(maintable.element[ x ].name, funcname) != 0 ) {
        maintable.element[ x ].kind = 0;
        maintable.element[ x ].address = 0;
        maintable.element[ x ].paranum = 0;
        maintable.element[ x ].value = 0;

        memset(maintable.element[ x ].name,'\0',sizeof(maintable.element[ x ].name));

        x--;
    }
    maintable.index = x + 1;
    return;
}



//������˵���� ::=  const���������壾;{ const���������壾;}  const int a=1;
void conststate(){
     while ( symid == CONSTTK ) {
         getsym();
         int ctpe;
         if ( symid == INTTK || symid == CHARTK ) {

             kind = ( symid == INTTK ) ? CINTET : CCHARET;
             address = 0;
             paranum = 0;

             ctpe = symid;
             getsym();
             constdef(ctpe);
             while ( symid == COMMASY ) {
                 getsym();
                 constdef(ctpe);
             }
             //�����������һ���ַ�һ���Ƿֺ�
             if ( symid != SEMICOLONSY ) {
                 error(SEMICOLONLACK,0);//CICVIFIRSP
                 cout<<"SEMICOLONLACK"<<endl;
                 //return;
                 //getsym();
                 continue;
             }
             getsym();
         }
         else{
             error(KEYWORDERROR,0);//CICVIFIRSP
          //����int����char
             cout<<"KEYWORDERROR"<<endl;
             return;
         }
     }

    // cout<<"This is a const statement!"<<endl;
     return;
 }

 //���������壾   ::=   int����ʶ��������������{,����ʶ��������������}

 //                           | char����ʶ���������ַ���{,����ʶ���������ַ���}


 void constdef( int ctpe ){
     //without int or char   a=123/char='a'
      char temp[ 200 ];
      //�ṹ��IDEN = XXXX
      if ( symid == IDEN ) {
          //name��¼��ʶ������
          strcpy(name, markstr.c_str() );
        // name = markstr;
          getsym();
          //if is '='
          if ( symid == ASSIGNSY ) {
              getsym();
              // if is '+' or '-'
              if ( symid == PLUSSY || symid == SUBSY ) {
                  int symb = symid;
                  getsym();
                  // if is integer
                  if ( ctpe == INTTK && symid == NUMET ) {
                      if ( symb == PLUSSY ) {
                          value = num;
                      }
                      else{
                          value = 0 - num;
                      }
                      pushtable(name, kind, value, address, paranum);
                      sprintf(temp, "%d", value);
                      fourvarcode("const", "int", temp, name);

                  }
              }
              // if is char
              else if ( symid == CHARET && ctpe==CHARTK ){
                value = stret.c_str()[1];
                pushtable(name, kind, value, address, paranum);
                sprintf(temp, "%d", stret.c_str()[1]);
                fourvarcode("const", "char", temp, name);
              }
              //�޷�������
              else if ( symid == NUMET && ctpe == INTTK ){
                value = num;
                pushtable(name, kind, value, address, paranum);
                sprintf(temp, "%d", value);
                fourvarcode("const", "int", temp, name);
              }
              else{
                  error(UNKNOWRIGHTSYM,0);//CS
                cout<<"UNKNOWRIGHTSYM"<<endl;
                return;
              }
          }
          //û�и�ֵ����
          else{
              error(CONSTNOTINIT);//CS
              cout<<"CONSTNOTINIT"<<endl;
              return;
          }
          num=0;
      }
      else
      {
         error(CSTDEFINEFAIL);//CS
         //δʶ�𵽱�ʶ��
         cout<<"CSTDEFINEFAIL"<<endl;
         return;
      }
      getsym();
      return;
  }


  //������ͷ����   ::=  int����ʶ���� | char����ʶ����
//ATTENTION: ��������int/char�ͱ�ʶ���Լ�������һ�����ʣ���
 void defhead()
 {

     if ( symid == INTTK || symid == CHARTK ) {
         kind = symid;
         getsym();

         if ( symid != IDEN ) {
            error(IDENTIFIERLACK, 1);//CLR  ����,��������( )
            cout<<"IDENLACK"<<endl;
            return;
         }
         else {

            strcpy(name, markstr.c_str());//��������������;
         }
     }
     else{
         error(KEYWORDERROR, 1);//CLR
         cout<<"KEYWORDERROR"<<endl;
         return;
     }
     getsym();
  //   cout<<"This is a define head!"<<endl;
     return;
}


//������˵����  ::= ���������壾;{���������壾;}
void varstate()
{
  //  while(symid==INTTK || symid==CHARTK){
         vardef();
        //ȱ��';'
        if ( symid != SEMICOLONSY ) {
            error(SEMICOLONLACK, 1);//SENTENFIRST
            cout<<"SEMICOLONLACK"<<endl;
            return;

        }
        getsym();
  //  }
   // cout<<"This is a variable statement!"<<endl;
    return;
}

//���������壾  ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]��) }
// int a/int a[22]/int a,b[11]
void vardef()
{
    if ( symid == INTTK || symid == CHARTK ) {
        kind=symid;
        getsym();
        if ( symid != IDEN ) {
            error(IDENTIFIERLACK, 2);//SENTENFIRST
            cout<<"IDENTIFIERLACK"<<endl;
            return;
        }
        else {
            strcpy(name, markstr.c_str());
        }
    }
    else{
        error(KEYWORDERROR, 2);//SENTENFIRST
        cout<<"KEYWORDERROR"<<endl;
        return;
    }
    getsym();

    int isArr = ( symid == LMBRASY );
    value = 0;
    address = 0;
    paranum = 0;

    if ( isArr ) {    //���������
        getsym();
        if ( symid != NUMET ) {
            error(IMMLACK);//SENTENFIRST
           //�˴������޷�������������������������������������������������������������������������������������
            cout<<"IMMLACK"<<endl;
            return;
        }
        else{
           if(num==0) {
                //���鶨���±�Ϊ0
                error(IMMLACK);//SENTENFIRST
                cout<<"ARRAYERROR"<<endl;
                return;
           }
            pushtable(name, kind, value, address, paranum, num);

            if ( kind == INTTK ){
                 char tempnum[20];
                 sprintf(tempnum,"%d",num);
                 fourvarcode("inta","  ",tempnum, name);

            }


            else if ( kind == CHARTK ){
                 char tempnum[20];
                sprintf(tempnum,"%d",num);
                fourvarcode("chara","  ",tempnum, name);
            }


            getsym();
            if ( symid != RMBRASY ){
                error(RIGHTBRACKLACK, 1);//SENTENFIRST
                cout<<"RMBRALACK"<<endl;
                return;
            }
             else{
                 getsym();
            }
        }
    }
    else{
            //��������
        pushtable(name, kind, value, address, paranum, isArr);
        if ( kind == INTTK )
            fourvarcode("int", "  ", "  ", name);

        else if ( kind == CHARTK )
            fourvarcode("char", "  ", "  ", name);

    }

    num=0;

    //��������Ƕ��ţ���ô��������δ����
    while ( symid == COMMASY ) {
//cout<<symid<<endl;
        getsym();

        if(symid != IDEN ){
            error(IDENTIFIERLACK, 2);
            cout<<"IDENTIFIERLACK"<<endl;//SENTENFIRST
            return;
        }

        strcpy(name, markstr.c_str());
        getsym();
        int isArr = ( symid == LMBRASY );

        if ( isArr ) {    //���������


            getsym();

            if ( symid != NUMET ) {
                error(IMMLACK);//SENTENFIRST
               //�˴������޷�������������������������������������������������������������������������������������
                cout<<"IMMLACK"<<endl;
                return;
            }
            else{
                pushtable(name, kind, value, address, paranum, num);

                if ( kind == INTTK ){
                    char tempnum [20];
                    sprintf(tempnum,"%d",num);

                    fourvarcode("inta", "  ", tempnum, name);
                }


                else if ( kind == CHARTK ){

                    char tempnum [20];
                    sprintf(tempnum,"%d",num);
                    fourvarcode("chara", "  ", tempnum, name);
                }


                getsym();
                if ( symid != RMBRASY ) {
                        error(RIGHTBRACKLACK);//SENTENFIRST
                    cout<<"RMBRALACK"<<endl;
                    return;
                }
                 else{
                     getsym();
                }
            }
        }
        else{
                //��������
            pushtable(name, kind, value, address, paranum, isArr);
            if ( kind == INTTK )
                fourvarcode("int", "  ", "  ", name);

            else if ( kind == CHARTK )
                fourvarcode("char", "  ", "  ", name);

        }
        num = 0;
    }
    return;
}

//������䣾    ::=  scanf ��(������ʶ����{,����ʶ����}��)��
void scanfsentence()
{
    char names[ 30 ];
    int s;
    getsym();
    if ( symid != LSBRASY ) {
            error(LEFTPARENTLACK, 1);//IWFLIRSPE
        cout<<"LSBRALACK"<<endl;
        return;
    }

    do{
        getsym();
        if ( symid != IDEN ) {
                error(IDENTIFIERLACK, 3);//IWFLIRSPE
            cout<<"IDENTIFIERLACK"<<endl;         //���Ϸ��ı�־��
            return;
        }

        strcpy(names, markstr.c_str());
        s = searchst(names, 0);
        if ( s < 0 ) {
                /*
            if ( s == -1 )
                error(VARNOTDEFINE, 1);            //δ�����־��
            else
                error(ASSIGNCONST, 1);*/
            return;

        }
        fourvarcode("scf", "  ", "  ", names);
        getsym();
    } while ( symid == COMMASY );

    if ( symid != RSBRASY ) {
            error(RIGHTPARENTLACK, 1);//IWFLIRSPE
        cout<<"RSBRALACK"<<endl;
        return;
    }

    getsym();
    return;
}


//��д��䣾    ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
void printfsentence()
{

    char str1[ 200 ] = "", str2[ 30 ] = ""; //str1���ַ�����str2�Ǳ��ʽ
    getsym();
    if ( symid != LSBRASY ) {
            error(LEFTPARENTLACK, 1);//IWFLIRSPE
        cout<<"LSBRALACK"<<endl;
        return;
    }

    getsym();

    if ( symid == STRINGET ) {
        strcpy(str1, stret.c_str());
        getsym();
        if ( symid == COMMASY ) {
            getsym();
            expression();
            strcpy(str2, exprevalue);
        }
    }
    else {
        expression();
        strcpy(str2, exprevalue);
    }


    if ( symid != RSBRASY ) {
            error(RIGHTPARENTLACK, 1);//IWFLIRSPE
        cout<<"RSBRALACK"<<endl;
        return;
    }

    string tempstr = (termtk == CHARTK) ? "char" : "int";


   // cout<< termtk<< "   1111111111111111111111111111111" <<endl;
    fourvarcode("prt", str1, str2, (char*)tempstr.data());
    getsym();
    return;
}

//��������䣾   ::=  return[��(�������ʽ����)��]
void returnsentence()
{
    char str3[ 30 ];
    getsym();
    if ( symid == LSBRASY ) {
        getsym();
        expression();//���ʽ�ӳ���
        strcpy(str3, exprevalue);

        if ( symid != RSBRASY ) {
                error(RIGHTPARENTLACK, 1);//IWFLIRSPE
            cout<<"RSBRALACK"<<endl;
            return;
        }
        returnnum++;
        fourvarcode("ret", "  ", "  ", exprevalue);
        getsym();
        return;
    }

    fourvarcode("ret", "  ", "  ", "  ");

    return;
}

////�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
void expression()
{
    termtk = 0;
    char str1[ 30 ],str2[ 30 ], str3[ 30 ];

    //�У����������
    if ( symid == PLUSSY || symid == SUBSY ) {
        termtk = INTTK;
        if ( symid == PLUSSY ) {
            getsym();
            item();//���������Ľ�������ȫ�ֱ���expervalue
            strcpy(str3, exprevalue);//����Ľ��������ʱ������λ��

        }
        else if ( symid == SUBSY ) {
            getsym();
            item();//��
            strcpy(str1, exprevalue);
            strcpy(str3, nextreg());
            fourvarcode("-", "0 ", str1, str3); //place3 = 0 - (place1)
        }
    }
    else {
        item();
        strcpy(str3, exprevalue);
     //   cout<<symid<<endl;
    }

    while ( symid == PLUSSY || symid == SUBSY ) {
        termtk = INTTK;
        strcpy(str1, str3);
        if ( symid == PLUSSY ) {
            getsym();
            item();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());
            fourvarcode("+", str1, str2, str3);
            continue;
        }
        else {
            getsym();
            item();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());
            fourvarcode("-", str1, str2, str3);
            continue;
        }
    }
    strcpy(exprevalue, str3);//�ѱ��ʽ������ֵ�����p֮��
    return;
}

//���     ::= �����ӣ�{���˷�������������ӣ�}
void item()
{
    char str1[ 200 ], str2[ 200 ], str3[ 200 ];
    factor();
    strcpy(str3, exprevalue);

    while ( symid == MULTSY || symid == DIVSY ) {
        termtk = INTTK;
        strcpy(str1, str3);
        if ( symid == MULTSY ) {
            getsym();
            factor();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());//������ʱ����
            fourvarcode("*", str1, str2, str3);
            continue;

        }
        else if ( symid == DIVSY ) {
            getsym();
            factor();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());
            fourvarcode("/", str1, str2, str3);
            continue;
        }
    }
    strcpy(exprevalue, str3);  //ÿһ���������ֵ���ڱ���exprevalue����
    return;
}


//�����ӣ�    ::= ����ʶ����������ʶ������[�������ʽ����]������������|���ַ��������з���ֵ����������䣾|��(�������ʽ����)��
void factor()
{
    int t = -1;
    char names[ 30 ], str3[ 30 ];

    //����ʶ����������ʶ������[�������ʽ����]�������з���ֵ����������䣾
    if ( symid == IDEN ) {
        strcpy(names, markstr.c_str());
        getsym();
        //����������ţ����ǣ��з���ֵ����������䣾
        if ( symid == LSBRASY ) {
            getsym();
            valueofpara();
            if ( symid != RSBRASY ) {
                    error(LEFTPARENTLACK, 2);//IWFXXXANE
                cout<<"RSBRALACK"<<endl;
                return;
            }

            t = searchst(names, 1);
            if ( t < 0 ) {
                /*if ( t == -1 * FUNCTIONRETURNNULL )
                    error(FUNCTIONRETURNNULL);   //�����޷���ֵ�����ܳ����ڱ��ʽ��
                else if ( t == -1 * FUNCTIONNOTFOUND )
                    error(FUNCTIONNOTFOUND, 1);
                    */
                return;
            }

            if(t==0){
            //        error(NONRETFUNCINEXPRE);//IWFXXXANE
           //     cout<<"NON RETURN FUNC IN EXPRESSION"<<endl;
            }


            if( termtk != INTTK )
                termtk == ( maintable.element[t].value == 1 )? INTTK:CHARTK;


            strcpy(str3, nextreg());//������ʱ����
            fourvarcode("call", names, "  ", str3);//�����õķ���ֵ�������ʱ��������
            strcpy(exprevalue, str3);


            getsym();
            cout<<"This is a returnfunction diaoyong sentence!"<<endl;
            return;
        }

        //����ʶ������[�������ʽ����]��
        else if ( symid == LMBRASY ){      //���������Ԫ��
            //?????????????????????????����Ԫ�������ű��ʽ��termtkֻ����INTTK
            getsym();
            //int tk = termtk;
            t = searchst(names, 0);
            int tk = searchkind(names,0);

            expression();
            if(termtk != INTTK)
            {
                cout<<"INVALID ARRAR INDEX"<<endl;
                error(IMMLACK);
                return;
               // exit(1);
            }
            if(termtk != INTTK){
                termtk = tk;
            }

            char temp[ 30 ];
            strcpy(temp, exprevalue);

            //����߽�����ж�

            if (isdigit(exprevalue[0])){
                int arrnums = 0;
                arrnums = arrnum(names);
                if (atoi(temp) >= arrnums){

                    printf("Warning: Line:%d index of array out of boundary, which should be less than %d.\n",linenum, atoi(temp));
                    error(ARRAYOUTBOUND);
                }
            }


            if ( symid != RMBRASY ) {
                    error(RIGHTBRACKLACK);//IWFXXXANE
                cout<<"RMBRALACK"<<endl;
                return;
            }
            else{
                strcpy(exprevalue, nextreg());
                fourvarcode("geta", names, temp, exprevalue);
                getsym();
            }
        }

        //����ǣ���ʶ����
        else{



          //  cout<< termtk << "   22222222222222222222222222" <<endl;

            t = searchst(names, 0);      //������ҵ���ʶ����Ӧ�ĵ�ַ
            if (t <0 ) {
                error(IDENTIFIERLACK, 4);//IWFXXXANE
                cout<<"IDENTIFIERLACK"<<endl;
                return;
            }
            if ( termtk!=INTTK) {
                termtk = searchkind(names,0);
            }

            /*
            if ( t < -1 ) {
                t = -t / 2;
            }
            */
            strcpy(exprevalue, names);
        }


        return;
    }

    //'('�����ʽ��')'
    else if ( symid == LSBRASY ) {
        getsym();
        expression();
        if ( symid != RSBRASY ) {
                error(RIGHTPARENTLACK, 2);//IWFXXXANE
            cout<<"RSBRALACK"<<endl;
            return;
        }
        getsym();
        return;
    }

    // �������������ַ���
    //�˴�Ҫ�������������ַ��Ĺ�ϵ������������������������������������������������������������������������������factclass,��������0�Ĺ�ϵ
    else if ( symid == PLUSSY || symid == SUBSY || symid == NUMET || symid == CHARET ) {

        if(symid == PLUSSY || symid == SUBSY){
            int t=(symid==PLUSSY)?1:-1;
            getsym();
            //��������
            termtk = INTTK;
            sprintf(exprevalue, "%d", t * num);
            num = 0;
            //�ٶ�һ��
            getsym();
            return;
        }

        else if(symid == NUMET){
            termtk = INTTK;
            sprintf(exprevalue, "%d",  num);
            //itoa(num,exprevalue,10);
     //       cout<< exprevalue<< "11111111111111111111111111111111111111111111111111111111" <<endl;
            num=0;
            getsym();
            return;
        }
        else if(symid == CHARET){
            if ( termtk != INTTK ) {
                termtk = CHARTK;
            }
            sprintf(exprevalue, "%d", stret.c_str()[1]);
           // cout<<exprevalue<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
            getsym();
            return;
        }

    }


    cout<<"EXPRESSIONERROR"<<endl;//IWFXXXANE
    error(EXPRESSIONERROR);                  //���ʽȱʧ�����
    return;
}


//��ֵ������   ::= �����ʽ��{,�����ʽ��}�����գ�
void valueofpara()
{
    int j = 0;
    vector<string> paraqueue;



    do {
        if ( symid == COMMASY )
            getsym();
        if ( symid == PLUSSY || symid == SUBSY || symid == IDEN || symid == LSBRASY || symid == CHARET || symid == NUMET ) {
            expression();
            paraqueue.push_back(exprevalue);
            tpkind[j]=termtk;
            j++;
        }
    } while ( symid == COMMASY );

    char tc[ 20 ];
    for ( int i = 0; i < paraqueue.size(); i++ ) {
        strcpy(tc, paraqueue[ i ].c_str());
        fourvarcode("fupa", "  ", "  ", tc);
    }
    paranum = j;
    paraqueue.~vector();

    return;
}


//������У�   ::= ������䣾��
//����䣾    ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾;

//                      | ���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|�������䣾����������䣾;
void sentencelist()
 {
     //sentence();
     //��䣬��������������µĶ�������ô�������
     while ( symid == IFTK || symid == FORTK || symid == LBBRASY
         || symid == IDEN || symid == RETURNTK || symid == SCANFTK || symid == PRINTFTK|| symid == SWITCHTK || symid == SEMICOLONSY ) {
         sentence();
     }
 }

 //��������䣾  ::=  if ��(������������)������䣾��else����䣾��
 void ifsentence()
 {
    char label1[ 20 ], label2[ 20 ], conditionvalue[ 30 ];
    strcpy(label1, nextlab());  //���������if��������label1��label1��if�Ľ�������
    strcpy(label2, nextlab());  //�������else����else�Ľ���λ����label2

     getsym();
     //�١�(��
     if ( symid != LSBRASY ) {
                error(LEFTPARENTLACK, 1);//IWFLIRSPE
         cout<<"LSBRALACK"<<endl;
         return;
     }
     getsym();
     //��������
     condition();

     strcpy(conditionvalue, exprevalue);  //���������ֵ��nowitem����,�˴�Ӧ���Ǹ���ʱ����$_x
     fourvarcode("jne", "  ", "  ", label1);  //�Ƚϣ�Ϊ�ٵ�ʱ����ת��label1

     if ( symid != RSBRASY ) {
            error(RIGHTPARENTLACK, 1);//IWFLIRSPE
         cout<<"RSBRALACK"<<endl;
         return;
     }
     getsym();
     sentence();

     fourvarcode("jmp", "  ", "  ", label2);//��ִ��else�Ĳ���,jump��lable2
     fourvarcode("lab:", "  ", "  ", label1);

     if ( symid == ELSETK ) {
         getsym();
         sentence();
     }
     fourvarcode("lab:", "  ", "  ", label2);
     return;
 }

//��������    ::=  �����ʽ������ϵ������������ʽ���������ʽ��
void condition()
 {

     char str1[ 30 ], str2[ 30 ];
     expression();
     strcpy(str1, exprevalue);  //����������һ�����ʽ

     //��ϵ�����
     if ( symid == LESSY || symid == LESEQSY || symid == GRTSY || symid == GRTEQSY || symid == EQUALSY || symid == NEQUALSY ) {
         if ( symid == LESSY ) {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             fourvarcode("<", str1, str2, "  ");
         }
         else if ( symid == LESEQSY ) {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             fourvarcode("<=", str1, str2, "  ");

         }
         else if ( symid == GRTSY ) {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             fourvarcode(">", str1, str2, "  ");

         }
         else if ( symid == GRTEQSY ) {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             fourvarcode(">=", str1, str2, "  ");

         }
         else if ( symid == EQUALSY ) {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             fourvarcode("==", str1, str2, "  ");

         }
         else if ( symid == NEQUALSY ) {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             fourvarcode("!=", str1, str2, "  ");

         }
        return;

     }
     strcpy(str2, "0");
     fourvarcode("!=", str1, str2,"  ");
     return;
 }


 //��ѭ����䣾::=  for��(������ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)����������)������䣾
  void loopsentence()
{
    int s = 0;
    char names[ 30 ], names1[ 30 ], names2[ 30 ], place2[ 30 ], place3[ 30 ];
    char label1[ 20 ], label2[ 20 ], label3[ 20 ] , op_flag[ 5 ];
    strcpy(label1, nextlab());
    strcpy(label2, nextlab());
    strcpy(label3, nextlab());

    if ( symid == FORTK ) {    //for|(...)
        getsym();
        if ( symid != LSBRASY ) {//for(|...)
                error(LEFTPARENTLACK, 1);//IWFLIRSPE
            cout<<"LSBRALACK"<<endl;
            return;
        }
        getsym();

        if ( symid != IDEN ) {    //for(i=x;...;...)
            error(UNACCEPTABLESENTENCE);//IWFLIRSPE
           cout<<"IDENTIFIERLACK"<<endl;
            return;
        }
        strcpy(names, markstr.c_str());

        getsym();
        if ( symid != ASSIGNSY ) {    //for(i=..;...;...)
            error(UNACCEPTABLESENTENCE);//IWFLIRSPE
           cout<<"ASSIGNLACK"<<endl;
            return;
        }


        s = searchst(names, 0);
        if ( s < 0 ) {     //����ǳ���
            /*if ( s == -1 ) error(VARNOTDEFINE, 1);  //"="����ǲ��Ϸ��ı�ʶ��
            else    error(ASSIGNCONST);*/
            return;
        }


        getsym();
        expression();    //for(i=a+b|;...;...)
        fourvarcode("=", exprevalue, "  ", names);

        if ( symid != SEMICOLONSY ) {
                error(SEMICOLONLACK, 2);//IWFLIRSPE
            cout<<"SEMICOLONLACK"<<endl;
            return;
        }
        getsym();//for(i=a+b;|...;...)

        fourvarcode("jmp", "  ", "  ", label3);
        fourvarcode("lab:", "  ", "  ", label1);

        condition();//for(i=a+b;...;|...)
        fourvarcode("jne", "  ", "  ", label2);
        fourvarcode("lab:", "  ", "  ", label3);

        if ( symid != SEMICOLONSY ) {
                error(SEMICOLONLACK, 2);//IWFLIRSPE
            cout<<"SEMICOLONLACK"<<endl;
            return;
        }
        getsym();        //for(i=a+b;...;|...)

        if ( symid != IDEN ) {
                 error(IDENTIFIERLACK, 3);//IWFLIRSPE
            cout<<"IDENTIFIERLACK"<<endl;
            return;
        }
        strcpy(names1, markstr.c_str());

        s = searchst(names1, 0);
        if ( s < 0 ) {     //����ǳ���
           /* if ( s == -1 ) error(VARNOTDEFINE, 1);  //"="����ǲ��Ϸ��ı�ʶ��
            else    error(ASSIGNCONST);
                */
            return;
        }

         //����ʶ����������ʶ����(+|-)��������!!!!!!!!!!!
        //��������    ::=  ���������֣��������֣���

        getsym();
        if ( symid != ASSIGNSY ) {
                error(UNACCEPTABLESENTENCE);//IWFLIRSPE
            cout<<"ASSIGNLACK"<<endl;
            return;
        }
        getsym();
        if ( symid != IDEN ) {
            error(IDENTIFIERLACK, 3);//IWFLIRSPE
            cout<<"IDENTIFIERLACK"<<endl;
            return;
        }

        strcpy(names2, markstr.c_str());
        s = searchst(names2, 0);
        if ( s < 0 ) {     //����ǳ���
          /*  if ( s == -1 ) error(VARNOTDEFINE, 1);  //"="����ǲ��Ϸ��ı�ʶ��
             else    error(ASSIGNCONST);*/
            return;
        }


        getsym();
        if ( symid != PLUSSY && symid != SUBSY ) {
                error(PLUSMINULACK);//IWFLIRSPE
            cout<<"PLUSSUBLACK"<<endl;
            return;
        }
        if ( symid == PLUSSY ) strcpy(op_flag, "+");
        else if ( symid == SUBSY ) strcpy(op_flag, "-");

        getsym();
        if ( symid != NUMET || num==0 ) {
                //�˴�Ҫ�жϵȺ��ұ߲�Ϊ0��������������������������������������������������������������������������������������������������
            error(UNKNOWRIGHTSYM, 1);     //�Ⱥ��ұ߲��ǺϷ�������
            cout<<"NUMLACK"<<endl;
            return;
        }

        sprintf(place2,"%d",num);

        getsym();
        if ( symid != RSBRASY ) {
            error(RIGHTPARENTLACK, 1);//IWFLIRSPE
            cout<<"RSBRALACK"<<endl;
            return;
        }

        getsym();
        sentence();

        strcpy(place3, nextreg());
        fourvarcode(op_flag, names2, place2, place3);
        fourvarcode("=", place3, "  ", names1);

        fourvarcode("jmp", "  ", "  ", label1);
        fourvarcode("lab:", "  ", "  ", label2);

        return;
    }
}

//�������䣾  ::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ����}��

//�������   ::=  ���������䣾{���������䣾}

//���������䣾  ::=  case��������������䣾

//��ȱʡ��   ::=  default : ����䣾

void casesentence()
{
    char label1[ 20 ],label2[20];
    char place1[50];
    int flg=0;

    strcpy(label2, nextlab());

    getsym();
    if ( symid != LSBRASY ) {
            error(LEFTPARENTLACK, 1);
        cout<<"LSBRALACK"<<endl;
        return;
    }
    getsym();
    expression();

    strcpy(place1,exprevalue);

    if ( symid != RSBRASY ) {
            error(RIGHTPARENTLACK, 1);
        cout<<"RSBRALACK"<<endl;
        return;
    }

    getsym();
    if ( symid != LBBRASY ) {
            error(LEFTBRACELACK, 1);
        cout<<"LBBRALACK"<<endl;
        return;
    }
    getsym();

    do{
        strcpy(label1, nextlab());

        char place2[50];

        if( symid == CASETK ){
            getsym();
            char ttt[50];
            if(symid == SUBSY || symid== PLUSSY|| symid == NUMET || symid == CHARET ){
                int t=1;

                if(symid == SUBSY || symid == PLUSSY){
                    t = (symid == SUBSY) ? -1 : 1 ;
                    getsym();
                }

                if(symid==NUMET)
                    sprintf(ttt,"%d",t*num);
                else{
                    //place2[0]='\'';
                    place2[0]=stret[1];
                   // place2[2]='\'';
                    place2[1]='\0';
                    sprintf(ttt,"%d",place2[0]);

                }

                fourvarcode("==", place1, ttt, "  ");
                //��ת���������
                fourvarcode("jne", "  ", "  ", label1);
                getsym();
                //���û��ð��
                if ( symid != COLONSY ) {
                        error(COLONLACK);
                    cout<<"COLONLACK"<<endl;
                    return;
                    break;
                }
                getsym();
                sentence();
                fourvarcode("jmp", "  ", "  ", label2);//��������������ִ��switch��������ಿ��
            }
            else{
                error(IMMLACK,1);//
                cout<<"IMMLACK"<<endl;
    //           return;
                ;
            }

            fourvarcode("lab:", "  ", "  ", label1);

        }
        else{

            error(KEYWORDERROR,2);//
            cout<<"CASETKLACK"<<endl;
      //      cout<< sym << "1111111111111111111111111111111111111111111" <<endl;
            return;
            break;
        }

    }while(symid==CASETK);

    //���û��default
    if ( symid != DEFAULTTK ) {
            error(KEYWORDERROR,2);
        cout<<"DEFAULTTKLAKE"<<endl;
        return;
    }
    getsym();

    //���û��ð��
    if ( symid != COLONSY ) {
            error(COLONLACK);
        cout<<"COLONLACK"<<endl;
        return;
    }
    getsym();
    sentence();

    if ( symid != RBBRASY ) {
        error(RIGHTBRACELACK, 1);
        cout<<"RBBRALACK"<<endl;
        return;
    }

    getsym();
    fourvarcode("lab:", "  ", "  ", label2);
    return;
}


//����䣾    ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾;

//                      | ���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|�������䣾����������䣾;
void sentence(){
    int s;

    //�������
    if ( symid == IFTK ) {
        ifsentence();
        cout<<"This is a if sentence!"<<endl;
        return;
    }

    //ѭ�����
    if ( symid == FORTK ) {
        loopsentence();
        cout<<"This is a loop sentence!"<<endl;
        return;
    }

       //������
    if( symid == SWITCHTK){
        casesentence();
        cout<<"This is a switch sentence!"<<endl;
        return;
    }

    //'{'�����'}'
    if ( symid == LBBRASY ) {
        getsym();
        sentencelist();
        //ȱ��}��
        if ( symid != RBBRASY ) {
                error(RIGHTBRACELACK, 1);
            cout<<"RBBRALACK"<<endl;
            return;
        }
        //��ȱ��}��
        getsym();
        cout<<"This is a sentence list!"<<endl;
        return;
    }

    //����������������ֵ��䣾;

    if ( symid == IDEN ) {
        int isVec = 0;
        char names[ MAXIDENLEN ];
        strcpy(names, markstr.c_str());
        char place2[ 100 ];

        getsym();
        //����ֵ��䣾;
        //����ֵ��䣾   ::=  ����ʶ�����������ʽ��   |    ����ʶ����'['<���ʽ>']'�������ʽ��

        if ( symid == LMBRASY ){ //���������
            /*
            s = searchst(names, 0);
            if ( s == -1 )
            {
           //     error(VARNOTDEFINE, 0);
      //          return;
            }
*/
            getsym();
            expression();
            strcpy(place2, exprevalue);
            //����Խ���飿����������������char���������int���������飿������

            if (isdigit(exprevalue[0])){
                int arrnums = 0;
                arrnums = arrnum(names);
                if (atoi(exprevalue) >= arrnums){
                    printf("Warning: Line:%d index of array out of boundary, which should be less than %d.\n",linenum, atoi(exprevalue));
                    error(ARRAYOUTBOUND);
                }
            }

            isVec = 1;
            if ( symid != RMBRASY ) {
                    error(RIGHTBRACKLACK, 1);
                cout<<"RMBRALACK"<<endl;
                return;
            }
            getsym();

        }
        if ( symid == ASSIGNSY ) {


            s = searchst(names, 0);
            if ( s < 0 ) {     //����ǳ���
              //  error(ASSIGNCONST);            //"="����ǲ��Ϸ��ı�ʶ��

             //   return;
            }
            int pretk = searchkind(names,0);
            if( maintable.element[s].kind == CINTET || maintable.element[s].kind == CCHARET ){
                error(ASSIGNCONST);            //"="����ǲ��Ϸ��ı�ʶ��
                cout<< "CONST CANNOT ASSIGN" <<endl;
                return;
            }


            getsym();
            expression();


            if(termtk == INTTK && pretk == CHARTK){
                error(TYPECHANGE);
                cout<< "WARNING:INT CANNOT ASSIGH TO CHAR!!!" << endl;
   //             return;
            }


            if ( isVec ){
                fourvarcode("[]=", names, place2, exprevalue);
            }
            else{
                fourvarcode("=", exprevalue, "  ", names);
            }


            if ( symid != SEMICOLONSY ) {
                     error(SEMICOLONLACK, 2);
                cout<<"SEMICOLONLACK"<<endl;
                return;
            }
            getsym();
            cout<<"This is a assign sentence!"<<endl;
            return;
        }

        //�����������
        if ( symid == LSBRASY ) {
            getsym();
            valueofpara();  //���㲢��¼������ֵ�Լ�����
            if ( symid != RSBRASY ) {
                    error(RIGHTPARENTLACK, 1);
                cout<<"RSBRALACK"<<endl;
                return;
            }

            //�麯����

            s = searchst(names, 1);
            if ( s <0 ) {
               // error(FUNCTIONNOTFOUND);
             //  cout<< markstr << " 33333333333333333333333333333333333333333333333333333333333333" <<endl;
     //           return;
            }
            else if ( s <0 ){
             //   error(FORMALPARANUMUNMATCH, 1);
     //           return;
            }
            fourvarcode("call", names, "  ", "  ");

            getsym();
            if ( symid != SEMICOLONSY ) {
                    error(SEMICOLONLACK, 2);
                cout<<"SEMICOLONLACK"<<endl;
                return;
            }
            getsym();
        }

        else {
                error(UNACCEPTABLESENTENCE);    //���Ϸ��ľ���
            cout<<"SENTENCEERROR"<<endl;    //���Ϸ��ľ���
            return;
        }
        cout<<"This is a function diaoyong sentence!"<<endl;
        return;
    }

    //�����
    if ( symid == SCANFTK ) {
        scanfsentence();
        if ( symid != SEMICOLONSY ) {
                error(SEMICOLONLACK, 2);
            cout<<"SEMICOLONLACK"<<endl;
            return;
        }
        getsym();
        cout<<"This is a scanf sentence!"<<endl;
        return;
    }


    //д���
    if ( symid == PRINTFTK ) {

        printfsentence();
        if ( symid != SEMICOLONSY ) {
                error(SEMICOLONLACK, 2);
                //cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"<< endl;
            cout<<"SEMICOLONLACK"<<endl;
            return;
        }
        getsym();
        cout<<"This is a printf sentence!"<<endl;
        return;
    }

    //�������
    if ( symid == RETURNTK ) {
        returnsentence();
        //��������ȱ������
        if ( symid != SEMICOLONSY ) {
                error(SEMICOLONLACK, 2);
            cout<<"SEMICOLONLACK"<<endl;
            return;
        }
        getsym();
        cout<<"This is a return sentence!"<<endl;
        return;
    }


    //�����
    if ( symid == SEMICOLONSY )
    {
        getsym();
        cout<<"This is a null sentence!"<<endl;
        return;
    }

    //���Ϸ��ľ���
    cout<<"SENTENCEERROR"<<endl;
    error(UNACCEPTABLESENTENCE);
    return;
}

//��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
void complexsentence()
{
    if ( symid == CONSTTK ) {
        //���ó���˵���ӳ���
        conststate();
        cout<<"This is a const statement!"<<endl;
    }

    //�ж��Ƿ�Ϊ����˵��
    if( symid == INTTK || symid == CHARTK ){
        while( symid == INTTK || symid == CHARTK )
            varstate();
        cout<<"This is a variable statement!"<<endl;
    }

    //�����
    sentencelist();
    return;
}

//������    ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
void program(){
   // getsym();

    if ( symid == CONSTTK ) {
        //���ó���˵���ӳ���
        conststate();
        //����Ƿֺ�,����������
        cout<<"This is a const statement!"<<endl;
    }

    int globvari=0;
    //����������з���ֵ�ĺ�������ͬ������ͷ�������Ԥ��3�����������ж�
    while ( symid == INTTK || symid == CHARTK ) {

        int tempid = symid;//�ָ��ֳ���
        char tempch = ch;//�ָ��ֳ���
        int temppin = pin;//�ָ��ֳ���

        int kind = symid;
        getsym();

        strcpy(name, markstr.c_str());//��������


        //�����ʶ���Ϸ�
        if ( symid != IDEN ) {
            error(IDENTIFIERLACK);//notdone
            cout<<"IDENTIFIERLACK"<<endl;
            continue;
//////////////////////////////////////////////////////////continue??????
        }
        getsym();
        //����Ƕ���,������,    �ֺţ����Ǳ�������
        if ( symid == COMMASY || symid == LMBRASY || symid == SEMICOLONSY ) {
            symid = tempid;
            pin = temppin;
            ch = tempch;
            //�ص�INTTK��CHARTK������ ������������
            varstate();
            globvari=1;
            continue;
        }

        else if( symid == LSBRASY ){
            symid = tempid;
            pin = temppin;
            ch = tempch;
            break;
        }
        else{
            error(DEFINEERROR);
            cout<<"STATEMENTERROR"<<endl;
            continue;
        }


    }
    if( globvari==1 )
        cout<<"This is a variable statement!"<<endl;

    /*
    if ( symid == CONSTTK ) {
        errnum++;
        printf("Error %d: Line %d, ��������λ�ô���\n", errnum, lnum);
        conststate();
        //����Ƿֺ�,����������
    }
    sislob = 0;
    */

    //�������岿��ͷ����
    if( symid != INTTK && symid != CHARTK && symid != VOIDTK ){

        error(FUNCDEFINEERROR);
        cout<<"FUNCTIONDEFINEERROR"<<endl;

    }

    int ismain=0;
    globdefine = 0;

    while ( symid == INTTK || symid == CHARTK || symid == VOIDTK ) {

        int temp = symid;

        //���з���ֵ�������壾  ::=  ������ͷ������(������������)��  ��{����������䣾��}��
        if ( symid == INTTK || symid == CHARTK ) {

            defhead();
            if ( symid != LSBRASY ) {
                error(LEFTPARENTLACK);//ICV
                cout<<"LSBRALACK"<<endl;
                continue;
            }

            varcnt = 0;         //��ʱ�������¿�ʼ����
            returnnum = 0;      //���Ƿ���ڷ���ֵ�ı�ǳ�ʼ����0��ʾ�޷���ֵ����δ����return
            kind = FUNCTION;    //��ʾ��ǰ�ĺ������ӳ��򣬼���������
            value = (symid== INTTK) ? 1 : 2;          //���ں�����˵��1��ʾ����ֵΪInt,0����ֵΪvoid
            address = 0;
            paranum = 0;
            pushtable(name, kind, value, address, paranum);
            strcpy(funcname, name);//����������
            if ( temp == INTTK ) fourvarcode("func", "int", "  ", funcname);
            if ( temp == CHARTK ) fourvarcode("func", "char", "  ", funcname);


            getsym();
            parametertable();
            //ȱ'('��������')'����С����
            if ( symid != RSBRASY ){
                    cout<< "bbbbbbbbbbbb"<<endl;
                error(RIGHTPARENTLACK);//ICV
                cout<<"RSBRALACK"<<endl;
                continue;
            }
            getsym();
            //ȱ'{'��������䣾'}'���������
            if ( symid != LBBRASY ) {
                error(LEFTBRACELACK);//ICV
                cout<<"LBBRALACK"<<endl;
                continue;
            }
            getsym();
            complexsentence();
            //ȱ'{'��������䣾'}'���Ҵ�����
            if ( symid != RBBRASY ) {
                error(RIGHTBRACELACK);//ICV
                cout<<"RBBRALACK"<<endl;
                continue;
            }

            //����ȱ�ٷ���ֵ
            if ( returnnum == 0 ) {
                error(NONERETURN);//ICV
                cout<<"RETURNVALUELACK"<<endl;
                continue;
            }

            cout<<"This is a return function statement!"<<endl;
            getsym();
            fourvarcode("end", "  ", "  ", funcname);//����������Ԫʽ����
            popst();//������ű�������յ��Ǵ��Ӻ����ķ��ű�,������Ӻ�����

        }

        //���޷���ֵ�������壾 ::= void����ʶ������(������������)����{����������䣾��}��
        if ( symid == VOIDTK ) {
            getsym();



            if ( symid != IDEN && symid !=MAINTK) {
                error(IDENTIFIERLACK);//ICV
                cout<<"IDENTIFIERLACK"<<endl;
                continue;
            }

            varcnt = 0;//��ʱ�������¿�ʼ����


          //  getsym();
            if (symid == MAINTK){
                ismain=1;
                break;
            }

            strcpy(name, markstr.c_str());

            kind = FUNCTION;
            value = 0;
            address = 0;
            paranum = 0;
            pushtable(name, kind, value, address, paranum);
            strcpy(funcname, name);
            fourvarcode("func", "void", "  ", funcname);

            getsym();
            if ( symid != LSBRASY ) {
                error(LEFTPARENTLACK);//ICV
                cout<<"LSBRALACK"<<endl;
                continue;
            }
            getsym();
            parametertable();
            if ( symid != RSBRASY ) {
                    error(RIGHTPARENTLACK);//ICV
                cout<<"RSBRALACK"<<endl;
                continue;
            }
            getsym();
            if ( symid != LBBRASY ) {
                error(LEFTBRACELACK);//ICV
                cout<<"LBBRALACK"<<endl;
                continue;
            }
            getsym();
            complexsentence();
            if ( symid != RBBRASY ) {
                error(RIGHTBRACELACK);//ICV
                cout<<"RBBRALACK"<<endl;
                continue;
            }

            cout<<"This is a nonreturn function statement!"<<endl;
            getsym();

            fourvarcode("end", "  ", "  ", funcname);
            popst();

        }
    }


    if( symid == VOIDTK )
        getsym();

    //ȱ��main�ؼ���
    if(symid != MAINTK){
        error(MAINLACK);
        cout<<"FUNCTIONDEFINEERROR"<<endl;
    }

    varcnt = 0;        //��ʱ�������¿�ʼ����
    strcpy(name, "main");
    kind = FUNCTION;
    value = 0;
    address = 0;
    paranum = 0;
    pushtable(name, kind, value, address, paranum);
    strcpy(funcname, name);
    fourvarcode("func", "  ", "  ", funcname);

    //void main(
    getsym();
    if ( symid != LSBRASY ) {
            error(LEFTPARENTLACK);//ICV
        cout<<"LSBRALACK"<<endl;
        //continue;
    }
    //void main()


    getsym();
    if ( symid != RSBRASY ) {
             error(RIGHTPARENTLACK);//ICV
        cout<<"RSBRALACK"<<endl;
        //continue;
    }

    //void main(){
    getsym();
    if ( symid != LBBRASY ) {
            error(LEFTBRACELACK);//ICV
        cout<<"LBBRALACK"<<endl;
        //continue;
    }

    getsym();
    complexsentence();

    fourvarcode("end", "  ", "  ", funcname);

    if ( symid != RBBRASY ) {
            error(RIGHTBRACELACK);//ICV
        cout<<"RBBRALACK"<<endl;
        //continue;
    }

    cout<<"This is a main function!"<<endl;
    cout<<"This is the program!"<<endl;
    return;


}


//��������     ::=    ��������
//��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
void parametertable()
{
    int i = 0;     //��¼��������
    int temp;
    int tparakind[MAXPARANUM];
    if ( symid == INTTK || symid == CHARTK ) {
        do{
            if ( symid == COMMASY )
                getsym();
            temp = symid;
            defhead();    //��������������ͷ��ʱ��ȫ��ͬ������������������������������������

            kind = (temp == INTTK) ? PARAINT : PARACHAR ;     //4��ʾΪ��������
            value = 0;
            address = i;  //��ַΪi����������λ�ã���ַȫ��Ϊ��Ե�ַ��
            paranum = 0;
            pushtable(name, kind, value, address, paranum);  //����������������ű�

            if ( temp == INTTK ) {
                fourvarcode("para", "int", "  ", name);
                tparakind[i]=INTTK;
            }
            else if ( temp == CHARTK ){
                fourvarcode("para", "char", "  ", name);
                tparakind[i]=CHARTK;
            }
            i++;//����������һ
        } while ( symid == COMMASY );//����Ƕ��ţ����������Ĳ���

        paranum = i;//��ǰ�Ĳ�������
        updatepara(paranum,tparakind);//���뺯���Ĳ�������


    }

    else if (symid == RSBRASY){
        return;
    }

    else{
            error(RIGHTPARENTLACK);//ICV
        cout<<"PARAERROR"<<endl;
    }
/*
    for(int i=0;i<maintable.index;i++){
        cout<<maintable.element[i].kind<<"  "<<maintable.element[i].name<<"  "<<maintable.element[i].value<<endl;
    }

*/
    return;
}



 //����0���⣡��������������������������������������������������
