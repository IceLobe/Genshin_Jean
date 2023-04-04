#ifndef midi_h__
#define midi_h__

#include <stdio.h>
#include <string>
#include <vector>
#include <Windows.h>

struct Header_Chunk {
	unsigned char MThd[4];
	unsigned char header_length[4]; // 标头块的长度
	unsigned short format; // 0 = 单轨文件格式 1 = 多轨文件格式	2 = 多首歌曲文件格式（即一系列类型 0 文件）
	unsigned short NumTracks; // 该文件中有NumTracks个MTrk块。
	unsigned short division; // 增量计时的时间单位。如果值为正，则 表示每拍单位。例如，+96 表示 96 每拍滴答声。如果该值为负数，则增量时间以 SMPTE 为单位 兼容单位。(ms)
};

struct MTrk_Track {
	unsigned char v_time;
	unsigned int DeltaTime;
	unsigned char event;
	unsigned char type;
	unsigned char LengthOrStrength; // 长度或力度
	std::vector<unsigned char> data;
};

struct Track_Chunk {
	int MTrk;
	int length;	// 跟踪区块中此数字后面的字节数。
	std::vector<MTrk_Track> track_event; // 按顺序排列的跟踪事件。
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
	// Method:    ReadInt 读取4字节
	// FullName:  midi::ReadInt
	// Access:    public 
	// Returns:   int 
	// Qualifier: 
	//************************************
	int ReadInt();
	//************************************
	// Method:    ReadByte 读取1字节
	// FullName:  midi::ReadByte
	// Access:    public 
	// Returns:   unsigned char 
	// Qualifier: 
	//************************************
	unsigned char ReadByte();
	//************************************
	// Method:    ReadBytes 读取指定字节
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
	unsigned int m_nQuarterNote = 0; // 4分音符的微妙数
	
	Header_Chunk m_mtData = {};

	std::vector<std::vector<Mapping_Key>> m_vecMapKey;

	std::vector<Track_Chunk> ReadMTrks(int NumTracks);
};
#endif // midi_h__


