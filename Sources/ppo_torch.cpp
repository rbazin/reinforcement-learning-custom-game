#include <torch/torch.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <string>

namespace T = torch;
namespace nn = torch::nn;
namespace optim = torch::optim;
using namespace std;

class PPO_Memory
{
public:
    vector<double> states;
    vector<double> probs;
    vector<double> vals;
    vector<double> actions;
    vector<double> rewards;
    vector<double> dones;
    int batch_size;

    template <typename T>
    vector<T> arange(T begin, T end, int jump)
    {
        vector<T> myVector = {begin};
        int tmp = begin + jump;
        while (tmp < end)
        {
            myVector.push_back(tmp);
            tmp += jump;
        }
        return myVector;
    }

    // random generator function:
    static int myrandom(int i) { return rand() % i; }

public:
    PPO_Memory(int batch_size)
    {
        this->states = {};
        this->probs = {};
        this->vals = {};
        this->actions = {};
        this->rewards = {};
        this->dones = {};

        this->batch_size = batch_size;
    }

    auto generate_batches()
    {
        srand(time(NULL));

        long long n_states = this->states.size();

        vector<long long> batch_start = arange(0LL, n_states, this->batch_size);

        vector<long long> indices = arange(0LL, n_states, 1);

        random_shuffle(indices.begin(), indices.end(), myrandom);

        vector<vector<long long>> batches = {};
        for (auto const batch : batch_start)
        {
            vector<long long>::const_iterator first = indices.begin() + batch;
            vector<long long>::const_iterator last;
            indices.begin() + batch + batch_size < indices.end() ? last = indices.begin() + batch + batch_size : last = indices.end();

            vector<long long> newVec(first, last);
            batches.push_back(newVec);
        }

        return make_tuple(this->states,
                          this->actions,
                          this->probs,
                          this->vals,
                          this->rewards,
                          this->dones,
                          batches); // tie(x, c, v, b, f, g, test) = ppo_memory.generate_batches()
    }

    void store_memory(double state, double action, double probs, double vals, double reward, double done)
    {
        this->states.push_back(state);
        this->actions.push_back(action);
        this->probs.push_back(probs);
        this->vals.push_back(vals);
        this->rewards.push_back(reward);
        this->dones.push_back(done);
    }

    void clear_memory()
    {
        this->states = {};
        this->actions = {};
        this->probs = {};
        this->vals = {};
        this->rewards = {};
        this->dones = {};
    }
};

class ActorNetwork : public nn::Module
{
public:
    string checkpoint_file;
    nn::Sequential actor;
    optim::Adam *optimizer;
    T::Device device;

    ActorNetwork(long int n_actions, int input_dims, double alpha, long int fc1_dims, long int fc2_dims, string chkpt_dir = "./tmp/ppo") : Module()
    {
        this->checkpoint_file = chkpt_dir;
        this->checkpoint_file += "/actor_torch_ppo";
        actor = nn::Sequential(
            nn::Linear(input_dims, fc1_dims),
            nn::ReLU(),
            nn::Linear(fc1_dims, fc2_dims),
            nn::ReLU(),
            nn::Linear(fc2_dims, n_actions),
            nn::Softmax() // Comment on met dim = -1 en argument ?
        );
        this->optimizer = new optim::Adam(this->parameters(), torch::optim::AdamOptions(alpha));
        this->device = T::kCPU;
        this->actor->to(device);
    }
    T::Tensor forward(T::Tensor input)
    {
    }
};