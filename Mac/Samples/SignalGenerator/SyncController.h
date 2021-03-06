/* -LICENSE-START-
** Copyright (c) 2009 Blackmagic Design
**
** Permission is hereby granted, free of charge, to any person or organization
** obtaining a copy of the software and accompanying documentation covered by
** this license (the "Software") to use, reproduce, display, distribute,
** execute, and transmit the Software, and to prepare derivative works of the
** Software, and to permit third-parties to whom the Software is furnished to
** do so, all subject to the following:
** 
** The copyright notices in the Software and this entire statement, including
** the above license grant, this restriction and the following disclaimer,
** must be included in all copies of the Software, in whole or in part, and
** all derivative works of the Software, unless such copies or derivative
** works are solely in the form of machine-executable object code generated by
** a source language processor.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
** -LICENSE-END-
*/
//
//  SyncController.h
//  Signal Generator
//

#import <Cocoa/Cocoa.h>
#import "DeckLinkAPI.h"
#import "SignalGenerator3DVideoFrame.h"

enum OutputSignal
{
	kOutputSignalPip		= 0,
	kOutputSignalDrop		= 1
};

class PlaybackDelegate;

@interface SyncController : NSObject {
	IBOutlet NSButton*			startButton;

	IBOutlet NSPopUpButton*		outputSignalPopup;
	IBOutlet NSPopUpButton*		audioChannelPopup;
	IBOutlet NSPopUpButton*		audioSampleDepthPopup;
	IBOutlet NSPopUpButton*		videoFormatPopup;
	IBOutlet NSPopUpButton*		pixelFormatPopup;

	IBOutlet NSView*			previewView;
	
	PlaybackDelegate*			playerDelegate;
	
	BOOL						running;
	IDeckLink*					deckLink;
	IDeckLinkOutput*			deckLinkOutput;
	
	uint32_t					frameWidth;
	uint32_t					frameHeight;
	BMDTimeValue				frameDuration;
	BMDTimeScale				frameTimescale;
	uint32_t					framesPerSecond;
	SignalGenerator3DVideoFrame*	videoFrameBlack;
	SignalGenerator3DVideoFrame*	videoFrameBars;
	uint32_t					totalFramesScheduled;
	//
	OutputSignal				outputSignal;
	void*						audioBuffer;
	uint32_t					audioBufferSampleLength;
	uint32_t					audioSamplesPerFrame;
	uint32_t					audioChannelCount;
	BMDAudioSampleRate			audioSampleRate;
	uint32_t					audioSampleDepth;
	uint32_t					totalAudioSecondsScheduled;
}

- (IBAction)toggleStart:(id)sender;
- (void)enableInterface:(BOOL)enable;

- (void)startRunning;
- (void)stopRunning;
- (void)scheduleNextFrame:(BOOL)prerolling;
- (void)writeNextAudioSamples;

@end

class PlaybackDelegate : public IDeckLinkVideoOutputCallback, public IDeckLinkAudioOutputCallback
{
private:
	int32_t						m_refCount;
	SyncController*				mController;
	IDeckLinkOutput*			mDeckLinkOutput;
	
public:
	PlaybackDelegate (SyncController* owner, IDeckLinkOutput* deckLinkOutput);
	
	// IUnknown
	virtual HRESULT		QueryInterface (REFIID iid, LPVOID *ppv);
	virtual ULONG		AddRef ();
	virtual ULONG		Release ();
	
	// IDeckLinkVideoOutputCallback
	virtual HRESULT		ScheduledFrameCompleted (IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result);
	virtual HRESULT		ScheduledPlaybackHasStopped ();
	
	// IDeckLinkAudioOutputCallback
	virtual HRESULT		RenderAudioSamples (bool preroll);
};

void	FillSine (void* audioBuffer, uint32_t samplesToWrite, uint32_t channels, uint32_t sampleDepth);
void	FillColourBars (IDeckLinkVideoFrame* theFrame, bool reversed);
void	FillBlack (IDeckLinkVideoFrame* theFrame);
void	ScheduleNextVideoFrame (void);
int		GetBytesPerPixel (BMDPixelFormat pixelFormat);
