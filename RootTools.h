#pragma once
#include "TH1F.h"
#include "TH1.h"
#include "TH2D.h"
#include "TH1D.h"
#include <iostream>

class RootTools
{
public:
	// Divides two histograms and returns the result
	static TH1* takeRatio(TH1*hist1, TH1*hist2, const char* title = "ratio", const char* xTitle = "", const char* yTitle="") {
		TH1 *ratio = (TH1*)hist1->Clone(title);
		ratio->Divide(hist2);
		ratio->SetTitle(title);
		ratio->GetXaxis()->SetTitle(xTitle);
		ratio->GetYaxis()->SetTitle(yTitle);
		ratio->Draw();
		return ratio;
	}

	// Rebins a histogram according to the bins of another one
	static TH1* synchronizeBins(TH1* histToRebin, TH1* comparisonHist, const char* newName) {
		std::vector<double_t> bins = extractBins(comparisonHist);
		TH1* rebinned = histToRebin->Rebin(bins.size() - 1, newName, bins.data());
		return rebinned;
	}

	static std::vector<double_t> extractBins(TH1* hist) {
		const int nBins = hist->GetNbinsX();
		std::vector<double_t> bins(nBins + 1);
		for (int i = 0; i < nBins + 1; i++) {
			bins[i] = hist->GetXaxis()->GetBinLowEdge(i + 1);
		}

		return bins;
	}

	static std::vector<double_t> getEqualBins(std::vector<double_t> data, int nBins=30) {
		double_t min = *std::min_element(data.begin(), data.end());
		double_t max = *std::max_element(data.begin(), data.end());
		std::vector<double_t> bins(nBins + 1);
		double_t binWidth = (max - min) / nBins;
		for (int i = 0; i < nBins + 1; ++i) {
			bins[i] = min + i * binWidth;
		}
		return bins;
	}

	static TH2D binAgainstPt(const char* name, const char* title, const char* xTitle, const char* yTitle, std::vector<double_t> pts, std::vector<double_t> values, TH1* modelHist) {
		int nYBins = 30;
		std::vector<double_t> xBins = extractBins(modelHist);
		std::vector<double_t> yBins = getEqualBins(values, nYBins);

		TH2D hist(name, title, xBins.size() - 1, xBins.data(), yBins.size() - 1, yBins.data());
		for (int i = 0; i < pts.size(); ++i) {
			hist.Fill(pts[i], values[i]);
		}
		hist.GetXaxis()->SetTitle(xTitle);
		hist.GetYaxis()->SetTitle(yTitle);
		return hist;
	}

	static TH1D binPt(const char* name, const char* title, const char* xTitle, std::vector<double_t> pts, TH1* modelHist) {
		std::vector<double_t> bins = extractBins(modelHist);

		TH1D hist(name, title, bins.size() - 1, bins.data());
		for (double_t pt : pts) {
			hist.Fill(pt);
		}

		hist.GetXaxis()->SetTitle(xTitle);

		return hist;
	}
};

