#ifndef _my_multithread_h
#define _my_multithread_h
#pragma once




//Ӱ����POSƥ���̺߳���
void WINAPI MultiThreadImgPosMatch(LPVOID p);
//�����������̺߳���
void WINAPI MultiThreadCmrDistortionCorrect(LPVOID p);
//���к���
void WINAPI MultiThreadReOrderStrips(LPVOID p);
//����Ӱ����Ч��
void WINAPI MultiThreadSetImgValidState(LPVOID p);
//��������
void WINAPI MultiThreadFlyQuaChkFun(LPVOID p);
//����Ӱ������
void WINAPI MultiThreadCreateImgTexture(LPVOID p);
//����Ӱ��ͶӰȡVBO
//void WINAPI MultiThreadCreateCqProjectVbo(LPVOID p);


#endif