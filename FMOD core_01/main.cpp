#define NOMINMAX
#include <Windows.h>

#include "fmod.hpp"
#include <string_view>
#include <iostream>

// FMOD 에러코드를 에러 문자열로 변환
constexpr std::string_view _fmod_errorString(FMOD_RESULT result)
{
	constexpr std::string_view errorString[] =
	{
		"FMOD_OK",
		"FMOD_ERR_BADCOMMAND",
		"FMOD_ERR_CHANNEL_ALLOC",
		"FMOD_ERR_CHANNEL_STOLEN",
		"FMOD_ERR_DMA",
		"FMOD_ERR_DSP_CONNECTION",
		"FMOD_ERR_DSP_DONTPROCESS",
		"FMOD_ERR_DSP_FORMAT",
		"FMOD_ERR_DSP_INUSE",
		"FMOD_ERR_DSP_NOTFOUND",
		"FMOD_ERR_DSP_RESERVED",
		"FMOD_ERR_DSP_SILENCE",
		"FMOD_ERR_DSP_TYPE",
		"FMOD_ERR_FILE_BAD",
		"FMOD_ERR_FILE_COULDNOTSEEK",
		"FMOD_ERR_FILE_DISKEJECTED",
		"FMOD_ERR_FILE_EOF",
		"FMOD_ERR_FILE_ENDOFDATA",
		"FMOD_ERR_FILE_NOTFOUND",
		"FMOD_ERR_FORMAT",
		"FMOD_ERR_HEADER_MISMATCH",
		"FMOD_ERR_HTTP",
		"FMOD_ERR_HTTP_ACCESS",
		"FMOD_ERR_HTTP_PROXY_AUTH",
		"FMOD_ERR_HTTP_SERVER_ERROR",
		"FMOD_ERR_HTTP_TIMEOUT",
		"FMOD_ERR_INITIALIZATION",
		"FMOD_ERR_INITIALIZED",
		"FMOD_ERR_INTERNAL",
		"FMOD_ERR_INVALID_FLOAT",
		"FMOD_ERR_INVALID_HANDLE",
		"FMOD_ERR_INVALID_PARAM",
		"FMOD_ERR_INVALID_POSITION",
		"FMOD_ERR_INVALID_SPEAKER",
		"FMOD_ERR_INVALID_SYNCPOINT",
		"FMOD_ERR_INVALID_THREAD",
		"FMOD_ERR_INVALID_VECTOR",
		"FMOD_ERR_MAXAUDIBLE",
		"FMOD_ERR_MEMORY",
		"FMOD_ERR_MEMORY_CANTPOINT",
		"FMOD_ERR_NEEDS3D",
		"FMOD_ERR_NEEDSHARDWARE",
		"FMOD_ERR_NET_CONNECT",
		"FMOD_ERR_NET_SOCKET_ERROR",
		"FMOD_ERR_NET_URL",
		"FMOD_ERR_NET_WOULD_BLOCK",
		"FMOD_ERR_NOTREADY",
		"FMOD_ERR_OUTPUT_ALLOCATED",
		"FMOD_ERR_OUTPUT_CREATEBUFFER",
		"FMOD_ERR_OUTPUT_DRIVERCALL",
		"FMOD_ERR_OUTPUT_FORMAT",
		"FMOD_ERR_OUTPUT_INIT",
		"FMOD_ERR_OUTPUT_NODRIVERS",
		"FMOD_ERR_PLUGIN",
		"FMOD_ERR_PLUGIN_MISSING",
		"FMOD_ERR_PLUGIN_RESOURCE",
		"FMOD_ERR_PLUGIN_VERSION",
		"FMOD_ERR_RECORD",
		"FMOD_ERR_REVERB_CHANNELGROUP",
		"FMOD_ERR_REVERB_INSTANCE",
		"FMOD_ERR_SUBSOUNDS",
		"FMOD_ERR_SUBSOUND_ALLOCATED",
		"FMOD_ERR_SUBSOUND_CANTMOVE",
		"FMOD_ERR_TAGNOTFOUND",
		"FMOD_ERR_TOOMANYCHANNELS",
		"FMOD_ERR_TRUNCATED",
		"FMOD_ERR_UNIMPLEMENTED",
		"FMOD_ERR_UNINITIALIZED",
		"FMOD_ERR_UNSUPPORTED",
		"FMOD_ERR_VERSION",
		"FMOD_ERR_EVENT_ALREADY_LOADED",
		"FMOD_ERR_EVENT_LIVEUPDATE_BUSY",
		"FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH",
		"FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT",
		"FMOD_ERR_EVENT_NOTFOUND",
		"FMOD_ERR_STUDIO_UNINITIALIZED",
		"FMOD_ERR_STUDIO_NOT_LOADED",
		"FMOD_ERR_INVALID_STRING",
		"FMOD_ERR_ALREADY_LOCKED",
		"FMOD_ERR_NOT_LOCKED",
		"FMOD_ERR_RECORD_DISCONNECTED",
		"FMOD_ERR_TOOMANYSAMPLES",
	};
	return errorString[result];
}
constexpr bool _fmod_failed(FMOD_RESULT result) { return result != FMOD_RESULT::FMOD_OK; }
void _fmod_result_draw(FMOD_RESULT result) { std::cout << "FMOD Error : " << _fmod_errorString(result) << "(" << result << ")" << std::endl; }

int main()
{
	// 리턴값 임시저장 변수
	FMOD_RESULT fmodResult(FMOD_RESULT::FMOD_OK);

	// FMOD core 본체
	FMOD::System* system(nullptr);

	// 시스템 생성
	fmodResult = FMOD::System_Create(&system);
	if (_fmod_failed(fmodResult))
	{
		_fmod_result_draw(fmodResult);
		abort();
	}

	// 시스템 초기화
	fmodResult = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	if (_fmod_failed(fmodResult))
	{
		_fmod_result_draw(fmodResult);
		abort();
	}

	// 음원 샘플 경로
	constexpr std::string_view _path_drum = "../../../../FMOD Studio API Windows/api/core/examples/media/drumloop.wav";
	constexpr std::string_view _path_wave = "../../../../FMOD Studio API Windows/api/core/examples/media/wave.mp3";

	FMOD::Sound* sound(nullptr);
	FMOD::Sound* asyncLoadingSound(nullptr);
	// 음원 불러오기
	fmodResult = system->createSound(_path_drum.data(), FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, std::addressof(sound));
	if (_fmod_failed(fmodResult))
	{
		_fmod_result_draw(fmodResult);
		abort();
	}
	// 음원 비동기로 불러오기
	fmodResult = system->createSound(_path_wave.data(), FMOD_DEFAULT | FMOD_NONBLOCKING | FMOD_LOOP_OFF, nullptr, std::addressof(asyncLoadingSound));
	if (_fmod_failed(fmodResult))
	{
		_fmod_result_draw(fmodResult);
		abort();
	}

	// 음원이 불러와질때까지 대기
	FMOD_OPENSTATE openstate;
	do
	{
		unsigned int loadingPer(NULL);
		asyncLoadingSound->getOpenState(std::addressof(openstate), std::addressof(loadingPer), nullptr, nullptr);
		std::cout << "asyncSound in loading : " << loadingPer << "%" << std::endl;
	} while (openstate == FMOD_OPENSTATE::FMOD_OPENSTATE_LOADING);

	// 음원 재생
	FMOD::Channel* channel(nullptr);
	system->playSound(sound, nullptr, false, std::addressof(channel));

	while (!GetAsyncKeyState(VK_RETURN));

	// 음원 재생
	system->playSound(asyncLoadingSound, nullptr, false, std::addressof(channel));
	
	while (!GetAsyncKeyState(VK_ESCAPE));


	// 불러온 음원 릴리즈
	sound->release();
	asyncLoadingSound->release();

	// FMOD 릴리즈
	fmodResult = system->release();
	if (_fmod_failed(fmodResult))
	{
		_fmod_result_draw(fmodResult);
		abort();
	}
	system = nullptr;
	return 0;
}
