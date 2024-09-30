//
// Created by _amp_ on 8/3/24.
//

#include "CUniGenWriter.hh"
#include "TFile.h"

CUniGenWriter::CUniGenWriter(const std::string &fName) : outputFile(std::unique_ptr<TFile>(TFile::Open(fName.c_str(), "RECREATE"))),
outputTree(std::make_unique<TTree>()), curEvent(std::make_unique<UEvent>()), run(std::make_unique<URun>()), _runFilled(false), count(0) {
    outputTree->Branch("run", run.get());
    outputTree->Branch("events", curEvent.get());
}

CUniGenWriter::~CUniGenWriter() {
    outputTree->Write();
    outputFile->Save();
    outputFile->Close();
}

void CUniGenWriter::operator()(std::unique_ptr<cola::EventData> && data) {
    count++;
    const auto& iniState = data->iniState;
    const auto& particles = data->particles;

    // fill run data (only once)
    if (not _runFilled) {
        auto nuclA = cola::pdgToAZ(iniState.pdgCodeA);
        auto nuclB = cola::pdgToAZ(iniState.pdgCodeB);

        run = std::make_unique<URun>("", "COLA output, -1 fields mean no info in DO",
                   nuclA.first, nuclA.second, iniState.pZA,
                   nuclB.first, nuclB.second, iniState.pZB,
                   -1, -1, -1, -1, -1, -1, -1);

        _runFilled = true;
    }

    curEvent->SetEventNr(static_cast<Int_t>(count));
    curEvent->SetB(iniState.b);
    curEvent->SetParameters(static_cast<Int_t>(count), iniState.b, -1, -1, -1, -1, "COLA output, -1 fields mean no info in DO");

    // Add particles
    int i = 0;
    for (const auto particle: particles)
        curEvent->AddParticle(i++, particle.pdgCode, static_cast<int>(particle.pClass), -1, -1, -1, -1, nullptr,
                              particle.momentum.x, particle.momentum.y, particle.momentum.z, particle.momentum.e,
                              particle.position.x, particle.position.y, particle.position.z, particle.position.t,
                              1);

    // Fill tree and check for buffer
    outputTree->Fill();
    if (not (count % buffSize)) {
        outputTree->Write();
        outputTree->Clear();
    }
}