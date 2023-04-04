#ifndef midi_h__
#define midi_h__

#include <stdio.h>
#include <string>
#include <vector>
#include <Windows.h>

struct Header_Chunk {
	unsigned char MThd[4];
	unsigned char header_length[4]; // ��ͷ��ĳ���
	unsigned short format; // 0 = �����ļ���ʽ 1 = ����ļ���ʽ	2 = ���׸����ļ���ʽ����һϵ������ 0 �ļ���
	unsigned short NumTracks; // ���ļ�����NumTracks��MTrk�顣
	unsigned short division; // ������ʱ��ʱ�䵥λ�����ֵΪ������ ��ʾÿ�ĵ�λ�����磬+96 ��ʾ 96 ÿ�ĵδ����������ֵΪ������������ʱ���� SMPTE Ϊ��λ ���ݵ�λ��(ms)
};

struct MTrk_Track {
	unsigned char v_time;
	unsigned int DeltaTime;
	unsigned char event;
	unsigned char type;
	unsigned char LengthOrStrength; // ���Ȼ�����
	std::vector<unsigned char> data;
};

struct Track_Chunk {
	int MTrk;
	int length;	// ���������д����ֺ�����ֽ�����
	std::vector<MTrk_Track> track_event; // ��˳�����еĸ����¼���
};

struct Mapping_Key {
	int midi_note_code;
	int time_tick;
	bool isDwon;
};

class midi {
public:
	~midi() {
		if (fp)
			fclose(fp);
		fp = 0;
	}

	static midi& Get() {
		static midi instance;
		return instance;
	}
	unsigned int GetQuarterNote() {
		return m_nQuarterNote;
	}

	bool ReadMidiFile(const char* pChrMidiFile);

	Header_Chunk GetHeaderData() {
		return m_mtData;
	}
	unsigned int  GetTickTime();

	std::vector<std::vector<Mapping_Key>> GetMapKeyTable() {
		return m_vecMapKey;
	}
protected:
	long GetPos() {
		return ftell(fp);
	}
	//************************************
	// Method:    ReadInt ��ȡ4�ֽ�
	// FullName:  midi::ReadInt
	// Access:    public 
	// Returns:   int 
	// Qualifier: 
	//************************************
	int ReadInt();
	//************************************
	// Method:    ReadByte ��ȡ1�ֽ�
	// FullName:  midi::ReadByte
	// Access:    public 
	// Returns:   unsigned char 
	// Qualifier: 
	//************************************
	unsigned char ReadByte();
	//************************************
	// Method:    ReadBytes ��ȡָ���ֽ�
	// FullName:  midi::ReadBytes
	// Access:    public 
	// Returns:   std::vector<unsigned char> 
	// Qualifier: 
	// Parameter: int Len 
	//************************************
	std::vector<unsigned char> ReadBytes(int Len);

	std::string ReadString(int Len);

	Header_Chunk ReadMThdData();

	Track_Chunk ReadMTrkData();

private:
	midi() = default;
	FILE* fp = 0;

	unsigned int m_nTickUnit = 0;
	unsigned int m_nQuarterNote = 0; // 4��������΢����
	
	Header_Chunk m_mtData = {};

	std::vector<std::vector<Mapping_Key>> m_vecMapKey;

	std::vector<Track_Chunk> ReadMTrks(int NumTracks);
};
#endif // midi_h__


