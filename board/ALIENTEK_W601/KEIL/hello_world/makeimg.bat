@rem ����0: exe
@rem ����1: ����bin�ļ� ,ԭʼ�ļ�����ѹ�����ļ�
@rem ����2: ����ļ�(Ŀ�������ļ���
@rem ����3: �����ļ�����,0�Ǿɲ������ֵ�image�ļ���3���²������ֵ�image�ļ�
@rem ����4: �Ƿ�ѹ���ļ���0��plain�ļ���1��ѹ�������ļ�
@rem ����5: �汾���ļ�
@rem ����6�������ļ���FLASH��Ĵ��λ�ã����λ�ã�
@rem ����7����������ļ�����λ�ã����λ�ã�
@rem ����8��ԭʼbin�ļ�

@echo off

@rem ����bin�ļ����֣����������MDKħ������->Output->Name of Executable������
set PATH_NAME=TencentOS_tiny
@rem ����bin�ļ�����·��
set PATH_OUT=..\OBJ
@rem ����TOOLS·��
set PATH_TOOLS=..\TOOLS
@rem ����BIN���·��(��W601�����õ��ļ�·��)
set PATH_BIN=..\BIN

copy 	%PATH_OUT%\%PATH_NAME%.bin	%PATH_BIN%
cd 	%PATH_TOOLS%

copy	%PATH_BIN%\version.txt 	%PATH_BIN%\version_bk.txt

makeimg.exe 	"%PATH_BIN%\%PATH_NAME%.bin" 	"%PATH_BIN%\%PATH_NAME%.img"	0 0	"%PATH_BIN%\version.txt" 90000 10100
makeimg_all.exe 	"%PATH_BIN%\secboot.img" 	"%PATH_BIN%\%PATH_NAME%.img" 	"%PATH_BIN%\%PATH_NAME%.FLS"
@del "%PATH_BIN%\%PATH_NAME%.img"


