#include <iostream>
#include <cstring>
#include <Windows.h>
#include "Gomoku_AI.h"

/*******************游戏部分******************/
class Gomoku_Play : public Gomoku_AI
{
private:
    int r, c;
public:
    void Illegal_Instruction();///非法操作提示
    int Get_Number();///
    void Pause_2();///停顿一下
    void Get_Key();///键入坐标
    void Start_Game();///开始游戏
};

int Gomoku_Play::Get_Number()
{
    int a = 0;
    char ch;
    while((ch = getchar()) != ' ' && ch != '\n')
    {
        if(ch >= '0' && ch <= '9')
            a = a * 10 + ch - '0';
        else
            RETURN_ERROR;
    }
    if(a > 0 && a <= 15)
        return a;
    RETURN_ERROR;
}

void Gomoku_Play::Illegal_Instruction()
{
    cout << "Ileagal! Please input again!" << endl;
}

void Gomoku_Play::Pause_2()
{;
    cout << "Please press any key to continue..." << endl;
    getchar();
    getchar();
}

void Gomoku_Play::Get_Key()
{
    cout << "Please input two numbers:" << endl;
    while((r = Get_Number()) == -1)
        system("cls"), Print_Board(), Illegal_Instruction();
    while((c = Get_Number()) == -1)
        system("cls"), Print_Board(), Illegal_Instruction(), printf("Now the program has got row: %d\n", r);
    while(!CheckValidity(r, c))
    {
        while((r = Get_Number()) == -1)
            system("cls"), Print_Board(), Illegal_Instruction();
        while((c = Get_Number()) == -1)
            system("cls"), Print_Board(), Illegal_Instruction(), printf("Now the program has got row: %d\n", r);
    }
}

void Gomoku_Play::Start_Game()
{
    OneStep CPTMV;
    int val;
    LetCPTBe(WHITE);
    system("cls");
    int CountSteps = 1;
    ///AI下棋
    Print_Board();
    //PrintCopyBoard();
    do
    {
        Get_Key();
        PutChess(r, c);
        RefreshBoard();///更新棋盘
        system("cls");
        Print_Board();
        //PrintValidityBlock();
        //PrintCopyBoard();
        //printf("Score: %d\n", EvaluateBoard());
        if(CheckWinner(r, c))
            break;
        val = AlphaBeta(MSEARCH, -INF, INF);
        Changeval(val);
        CPTMV = getBestMove();
        r = CPTMV.Row;
        c = CPTMV.Column;
        PutChess(r, c);
        RefreshBoard();///更新棋盘
        Refreshfac();
        system("cls");
        Print_Board();
        printf("Evaluation: %d\nPut chess : %d,%d\n", val, r, c);
        if(val == INF)
            printf("CPT: I have 95%% to win!\n");
        if(val == -INF)
            printf("CPT: I think you may win...\n");
    }while(!CheckWinner(r, c));
    cout << "Game Over! The winner is " << (GetNowPlayer() ? "Human!" : "Computer!") << endl;
    Print_Manual();
    //cout << "Evaluate:" << EvaluateBoard() << endl;
    Pause_2();
}
/*******************游戏部分******************/

/*******************测试部分******************/
int a[10];
int ed, cnt;
Gomoku_AI ai_test;
void dfs(int i)
{
    if(i == ed)
    {
        a[ed] = WHITE;
        cnt += 1;
        for(register int k = 1; k <= ed; k += 1)
            if(a[k] == WHITE)
                cout << "○";
            else
                cout << "□";
        cout << endl;
        ai_test.EvaluateLine(a, ed, cnt);
        getchar();
        a[ed] = EMPTY;
        cnt -= 1;
        for(register int k = 1; k <= ed; k += 1)
            if(a[k] == WHITE)
                cout << "○";
            else
                cout << "□";
        cout << endl;
        ai_test.EvaluateLine(a, ed, cnt);
        getchar();
        return;
    }
    a[i] = WHITE;
    cnt += 1;
    dfs(i+1);
    a[i] = EMPTY;
    cnt -= 1;
    dfs(i+1);
}
/*******************测试部分******************/

int main()
{
    int op;
    /**
    set rr="HKCU\Console\%%SystemRoot%%_system32_cmd.exe"
    reg delete %rr% /f>nul
    reg add %rr% /v "WindowPosition" /t REG_DWORD /d 0x00640104 /f>nul
    reg add %rr% /v "WindowSize" /t REG_DWORD /d 0x00190050 /f>nul
    reg add %rr% /v "ScreenBufferSize" /t REG_DWORD /d 0x00190050 /f>nul
    reg add %rr% /v "FullScreen" /t REG_DWORD /d 0x00000001 /f>nul
    reg add %rr% /v "FontSize" /t REG_DWORD /d 0x00100008 /f>nul*/
    /**
        Maybe we should test all type of chess
    */
    /**for(ed = 9; ed <= 9; ed += 1)
    {
        dfs(1);
    }
    return 0;*/
    system("title=Tzanse Gomoku");
    system("color f0");
    system("mode con cols=100 lines=50");
    while(1)
    {
        system("cls");
        cout << "Input a number please (Play-1, Exit-0):" << endl;
        cin >> op;
        if(op != 1 && op != 0)
        {
            cout << "Ileagal! Please input again!" << endl;
            continue;
        }
        if(!op)
        {
            cout << "Thank you for your playing!" << endl;
            break;
        }
        cout << "Game Start!" << endl;
        Gomoku_Play playing;
        playing.Pause_2();
        playing.Start_Game();
    }
    return 0;
}
