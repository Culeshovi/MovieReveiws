#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <cmath>


//Reads file , pre-process it and returns strings
std::string readFile(const std::string &filename)
{
    FILE *in=fopen(filename.c_str(), "r");
    fseek(in, 0, SEEK_END);
    size_t size = ftell(in);
    std::vector<char> buffer(size);
    rewind(in);
    fread(buffer.data(), 1, size, in);
    fclose(in);
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'!'), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'('), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),')'), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),','), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'"'), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'.'), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'?'), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'@'), buffer.end());
    buffer.erase(std::remove(buffer.begin(), buffer.end(),'|'), buffer.end());
    return std::string(buffer.begin(), buffer.end());
}

//Splits strings , pre-process it and returns vector of strings
std::vector <std::string> split(std::string file)
{
  std::stringstream ss(file);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> vstrings(begin, end);
  std::vector<std::string> stop_words{"ourselves", "hers", "between", "yourself", "but", "again", "there", "about", "once", "during", "out", "very", "having", "with", "they", "own", "an", "be", "some", "for", "do", "its", "yours", "such", "into", "of", "most", "itself", "other", "off", "is", "s", "am", "or", "who", "as", "from", "him", "each", "the", "themselves", "until", "below", "are", "we", "these", "your", "his", "through", "don", "nor", "me", "were", "her", "more", "himself", "this", "down", "should", "our", "their", "while", "above", "both", "up", "to", "ours", "had", "she", "all", "no", "when", "at", "any", "before", "them", "same", "and", "been", "have", "in", "will", "on", "does", "yourselves", "then", "that", "because", "what", "over", "why", "so", "can", "did", "not", "now", "under", "he", "you", "herself", "has", "just", "where", "too", "only", "myself", "which", "those", "i", "after", "few", "whom", "t", "being", "if", "theirs", "my", "against", "a", "by", "doing", "it", "how", "further", "was", "here", "than"};
  for(auto &i:stop_words)
    vstrings.erase(std::remove(vstrings.begin(), vstrings.end(),i), vstrings.end());
  return vstrings;
  
}






int main()
{

std::vector <std::vector <std::string>> v1;
std::vector <std::vector <std::string>> v2;




std::string path = "pos/";
  for(auto& p : std::filesystem::directory_iterator(path))
   {
      std::string file = p.path().filename();
      v1.push_back(split(readFile(path + file)));
       
   }

std::string path2 = "neg/";
    for(auto& p : std::filesystem::directory_iterator(path2))
    {
        std::string file = p.path().filename();
        v2.push_back(split(readFile(path2 + file)));
        
    }



std::map<std::string, double> positive;
std::map<std::string, double> negative;

//Train-Test Split
std::size_t const size=800; 
std::vector <std::vector <std::string>> v1_train(v1.begin(), v1.begin() + size);
std::vector <std::vector <std::string>> v1_test(v1.begin() + size, v1.end());

std::vector <std::vector <std::string>> v2_train(v2.begin(), v2.begin() + size);
std::vector <std::vector <std::string>> v2_test(v2.begin() + size, v2.end());


for (auto &i:v1_train){
for(auto &j:i){
auto k=positive.find(j);
if(k == positive.end())
  positive[j]=0;
}
}

for (auto &i:v2_train){
for(auto &j:i){
auto k=positive.find(j);
if(k == positive.end())
  positive[j]=0;
}
}

negative=positive;



for (std::map<std::string,double>::iterator it=positive.begin(); it!=positive.end(); ++it){
double num=1;
double deno=positive.size();
for(auto &i:v1_train){
  
    auto z=0;
    for(auto &j:i){
      
      if(j==it->first){
        num++;
        z++;
      }
      if (z==1){
        deno+=i.size();
      }
}
}
positive[it->first]=(num/deno);
}




for (std::map<std::string,double>::iterator it=negative.begin(); it!=negative.end(); ++it){
double num=1;
double deno=negative.size();
for(auto &i:v2_train){
  
    auto z=0;
    for(auto &j:i){
      
      if(j==it->first){
        num++;
        z++;
      }
      if (z==1){
        deno+=i.size();
      }
}
}
negative[it->first]=(num/deno);
}




std::vector <int> pred_p;
std::vector <int> true_p;



for(auto &i:v1_test){
  auto p1=log(0.5);
  auto p2=log(0.5);

  for(auto &j:i){
    auto k=positive.find(j);
    if(k != positive.end())
      p1+=log(positive[j]);
    
    auto q=negative.find(j);
    if(q != negative.end())
      p2+=log(negative[j]);

    
}


true_p.push_back(1);
if(p1<p2){
pred_p.push_back(1);
}

else{
pred_p.push_back(0);
}

}







for(auto &i:v2_test){
  auto p1=log(0.5);
  auto p2=log(0.5);

  for(auto &j:i){
    auto k=positive.find(j);
    if(k != positive.end())
      p1+=log(positive[j]);
    
    auto q=negative.find(j);
    if(q != negative.end())
      p2+=log(negative[j]);

    
}


true_p.push_back(0);
if(p1>p2){
pred_p.push_back(1);
}
else{
pred_p.push_back(0);
}

}



double acc=0;

for (int i=0;i<true_p.size();i++){
  if(true_p[i]==pred_p[i]){
    acc++;
  }
}

acc=(acc*100)/true_p.size();
std::cout<<"Test Accuracy is "<<acc<<std::endl;




}
