/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//extract parameters from audio processor value tree state
struct ChainSettings
{
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    int lowCutSlope{ 0 }, highCutSlope{ 0 };
};
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class SimpleeqAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleeqAudioProcessor();
    ~SimpleeqAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout()};

private:

    /* create alias names for commonly used functions in our dsp since juce has deep 
       nested stuff and it'll get ugly. */
    using Filter = juce::dsp::IIR::Filter<float>;

    /* using the juce processing chain we'll put four of these filters in a processor 
       chain which will allow it to process a single context and all of the audio automatically.*/
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    // Mono Chain: Lowcut -> Parametric -> HighCut
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    // need two instances of mono chain to have stereo processing
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowCut, 
        Peak, 
        HighCut
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleeqAudioProcessor)
};
