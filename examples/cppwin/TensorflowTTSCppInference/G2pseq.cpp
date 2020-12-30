#include "G2pseq.h"

MatrixXf G2pseq::encode(std::string word)
{
    std::vector<size_t> x;
    for (size_t i = 0; i < word.size(); i++) {
        auto search = g2idx.find(word.substr(i, 1));
        if (search != g2idx.end()) {
            x.push_back(search->second);
        }
        else {
            x.push_back(g2idx["<unk>"]);
        } 
    }
    x.push_back(g2idx["</s>"]);

    MatrixXf enc(x.size(), encEmb.cols());
    for (size_t i = 0; i < x.size(); i++) {
        enc.row(i) = encEmb.row(x[i]);
    }
    return enc;
}

MatrixXf G2pseq::sigmoid(MatrixXf& x)
{
    return (1 / (1 + (-x).array().exp())).matrix();
}

MatrixXf G2pseq::gru(MatrixXf& x, size_t steps, MatrixXf& wIh, MatrixXf& wHh, MatrixXf& bIh, MatrixXf& bHh, MatrixXf& h0)
{
    // x (nb_char, feat_dim)
    MatrixXf h = h0;
    MatrixXf outputs(steps, wHh.cols());
    for (size_t i = 0; i < steps; i++) {
        // x_t (1, feat_dim)
        MatrixXf x_t = x.block(i, 0, 1, x.cols());
        grucell(x_t, h, wIh, wHh, bIh, bHh);
        outputs.row(i) = h.row(0);
    }
    return outputs;
}

void G2pseq::grucell(MatrixXf& x, MatrixXf& h, MatrixXf& wIh, MatrixXf& wHh, MatrixXf& bIh, MatrixXf& bHh)
{
    // (batch, 256) * (256, 768) + (768, 1)
    MatrixXf rznIh = x * wIh.transpose() + bIh.transpose();
    MatrixXf rznHh = h * wHh.transpose() + bHh.transpose();

    int split = int(rznIh.cols() * 2 / 3);
    // (batch, split)
    MatrixXf rzIh = rznIh.block(0, 0, rznIh.rows(), split);
    // (batch, cols - split)
    MatrixXf nIh = rznIh.block(0, split, rznIh.rows(), rznIh.cols() - split);

    split = int(rznHh.cols() * 2 / 3);
    MatrixXf rzHh = rznHh.block(0, 0, rznHh.rows(), split);
    MatrixXf nHh = rznHh.block(0, split, rznHh.rows(), rznHh.cols() - split);

    MatrixXf rz = rzIh + rzHh;
    rz = sigmoid(rz);

    split = int(rz.cols() / 2);
    MatrixXf r = rz.block(0, 0, rz.rows(), split);
    MatrixXf z = rz.block(0, split, rz.rows(), rz.cols() - split);

    ArrayXXf n = (nIh.array() + r.array() * nHh.array()).tanh();

    // std::cout << nIh.array() + r.array() * nHh.array() << std::endl;

    h = ((1 - z.array()) * n + z.array() * h.array()).matrix();

    // return h;
}

G2pseq::G2pseq()
{
    std::string lower = "abcdefghijklmnopqrstuvwxyz";
    graphemes = { "<pad>", "<unk>", "</s>" }; 
    for (size_t i = 0; i < lower.size(); i++) {
        graphemes.push_back(lower.substr(i, 1));
    }
    
    phonemes = { "<pad>", "<unk>", "<s>", "</s>", "AA0", "AA1", "AA2", "AE0", "AE1", "AE2", "AH0", "AH1", "AH2", "AO0",
                "AO1", "AO2", "AW0", "AW1", "AW2", "AY0", "AY1", "AY2", "B", "CH", "D", "DH",
                "EH0", "EH1", "EH2", "ER0", "ER1", "ER2", "EY0", "EY1",
                "EY2", "F", "G", "HH",
                "IH0", "IH1", "IH2", "IY0", "IY1", "IY2", "JH", "K", "L",
                "M", "N", "NG", "OW0", "OW1",
                "OW2", "OY0", "OY1", "OY2", "P", "R", "S", "SH", "T", "TH",
                "UH0", "UH1", "UH2", "UW",
                "UW0", "UW1", "UW2", "V", "W", "Y", "Z", "ZH" };

    for (size_t i = 0; i < graphemes.size(); i++) {
        g2idx[graphemes[i]] = i;
        idx2g[i] = graphemes[i];
    }

    for (size_t i = 0; i < phonemes.size(); i++) {
        p2idx[phonemes[i]] = i;
        idx2p[i] = phonemes[i];
    }
}

G2pseq::~G2pseq()
{
}

bool G2pseq::initialize(std::string path)
{
    std::ifstream file_weights(path + "/g2p_s2s.json");
    Json::Value variables;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;

    if (!parseFromStream(builder, file_weights, &variables, &errs)) {
        std::cout << errs << std::endl;
        return EXIT_FAILURE;
    }

    // Load encoder parameters
    int col = variables["enc_emb"].size() / 29;
    //std::cout << col << std::endl;

    //encEmb.resize(29, col);
    for (int i = 0; i < 29; i++) {
        for (int j = 0; j < col; j++) {
            encEmb(i, j) = variables["enc_emb"][i * col + j].asFloat();
        }
    }

    col = variables["enc_w_ih"].size() / 768;
    //std::cout << col << std::endl;

    //encWIh.resize(768, col);
    for (int i = 0; i < 768; i++) {
        for (int j = 0; j < col; j++) {
            encWIh(i, j) = variables["enc_w_ih"][i * col + j].asFloat();
        }
    }

    col = variables["enc_w_hh"].size() / 768;
    //std::cout << col << std::endl;

    //encWHh.resize(768, col);
    for (int i = 0; i < 768; i++) {
        for (int j = 0; j < col; j++) {
            encWHh(i, j) = variables["enc_w_hh"][i * col + j].asFloat();
        }
    }

    //encBIh.resize(768, 1);
    for (int i = 0; i < 768; i++) {
        encBIh(i, 0) = variables["enc_b_ih"][i].asFloat();
    }

    //encBHh.resize(768, 1);
    for (int i = 0; i < 768; i++) {
        encBHh(i, 0) = variables["enc_b_hh"][i].asFloat();
    }

    // Load decoder parameters
    col = variables["dec_emb"].size() / 74;
    //std::cout << col << std::endl;

    //decEmb.resize(74, col);
    for (int i = 0; i < 74; i++) {
        for (int j = 0; j < col; j++) {
            decEmb(i, j) = variables["dec_emb"][i * col + j].asFloat();
        }
    }

    col = variables["dec_w_ih"].size() / 768;
    //std::cout << col << std::endl;

    //decWIh.resize(768, col);
    for (int i = 0; i < 768; i++) {
        for (int j = 0; j < col; j++) {
            decWIh(i, j) = variables["dec_w_ih"][i * col + j].asFloat();
        }
    }

    col = variables["dec_w_hh"].size() / 768;
    //std::cout << col << std::endl;

    //decWHh.resize(768, col);
    for (int i = 0; i < 768; i++) {
        for (int j = 0; j < col; j++) {
            decWHh(i, j) = variables["dec_w_hh"][i * col + j].asFloat();
        }
    }
   
    //decBIh.resize(768, 1);
    for (int i = 0; i < 768; i++) {
        decBIh(i, 0) = variables["dec_b_ih"][i].asFloat();
    }
    
    //decBHh.resize(768, 1);
    for (int i = 0; i < 768; i++) {
        decBHh(i, 0) = variables["dec_b_hh"][i].asFloat();
    }

    col = variables["fc_w"].size() / 74;
    //std::cout << col << std::endl;

    //fcW.resize(74, col);
    for (int i = 0; i < 74; i++) {
        for (int j = 0; j < col; j++) {
            fcW(i, j) = variables["fc_w"][i * col + j].asFloat();
        }
    }

    //fcB.resize(74, 1);
    for (int i = 0; i < 74; i++) {
        fcB(i, 0) = variables["fc_b"][i].asFloat();
    }

	return true;
}

std::string G2pseq::predict(std::string word)
{
    // (nb_word, 256)
    MatrixXf enc = encode(word);
    MatrixXf h0(1, encWHh.cols());
    h0.setZero();
    enc = gru(enc, word.size() + 1, encWIh, encWHh,
        encBIh, encBHh, h0);

    // <s>
    MatrixXf dec = decEmb.block(2, 0, 1, decEmb.cols());
    MatrixXf h = enc.block(enc.rows()-1, 0, 1, enc.cols());
    std::string preds;

    for (size_t i = 0; i < 20; i++) {
        grucell(dec, h, decWIh, decWHh, decBIh, decBHh);
        // (1, 74) + (74, 1) = (1, 74)
        MatrixXf logits = h * fcW.transpose() + fcB.transpose();
        int pred;
        logits.row(0).maxCoeff(&pred);
        if (pred == 3) {
            break;
        }

        auto search = idx2p.find(pred);
        if (search != idx2p.end()) {
            preds.append(idx2p[pred]);
        }
        else {
            preds.append("<unk>");
        }
        preds.append(" ");
        dec = decEmb.block(pred, 0, 1, decEmb.cols());
    }

    if (preds.size() > 0) {
        if (preds[preds.size() - 1] == ' ') {
            preds.pop_back();
        }
    }

    return preds;
}
