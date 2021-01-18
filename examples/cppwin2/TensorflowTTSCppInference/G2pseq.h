#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <Eigen/Dense>
#include "json/json.h"

using namespace Eigen;

class G2pseq {
private:
	std::vector<std::string> graphemes;
	std::vector<std::string> phonemes;
	std::unordered_map<std::string, size_t> g2idx;
	std::unordered_map<size_t, std::string> idx2g;
	std::unordered_map<std::string, size_t> p2idx;
	std::unordered_map<size_t, std::string> idx2p;

	MatrixXf encEmb = MatrixXf::Zero(29, 256);
	MatrixXf encWIh = MatrixXf::Zero(768, 256);
	MatrixXf encWHh = MatrixXf::Zero(768, 256);
	MatrixXf encBIh = MatrixXf::Zero(768, 1);
	MatrixXf encBHh = MatrixXf::Zero(768, 1);
	MatrixXf decEmb = MatrixXf::Zero(74, 256);
	MatrixXf decWIh = MatrixXf::Zero(768, 256);
	MatrixXf decWHh = MatrixXf::Zero(768, 256);
	MatrixXf decBIh = MatrixXf::Zero(768, 1);
	MatrixXf decBHh = MatrixXf::Zero(768, 1);
	MatrixXf fcW = MatrixXf::Zero(74, 256);
	MatrixXf fcB = MatrixXf::Zero(74, 1);

	MatrixXf encode(std::string word);
	void grucell(MatrixXf& x, MatrixXf& h,
		MatrixXf& wIh, MatrixXf&wHh,
		MatrixXf& bIh, MatrixXf&bHh);
	MatrixXf sigmoid(MatrixXf& x);
	MatrixXf gru(MatrixXf& x, size_t steps,
		MatrixXf& wIh, MatrixXf& wHh,
		MatrixXf& bIh, MatrixXf& bHh,
		MatrixXf& h0);

public:
	G2pseq();
	~G2pseq();
	bool initialize(std::string path);

	std::string predict(std::string word);
};