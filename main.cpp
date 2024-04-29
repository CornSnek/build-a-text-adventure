#include <iostream>
#include <fstream>
#include <cassert>
#include <random>
#include <vector>
#include <chrono>
enum class PathType;
struct PathOption;
struct Path;

struct PathArray{
  const PathOption* paths;
  size_t size;
};
//void* for any_data to be checked.
using BooleanFn=bool(*)(void*);
struct BooleanPath{
  BooleanFn boolean_fn;
  void* any_data;
  const Path* is_true;
  const Path* is_false;
  BooleanPath(){}
  BooleanPath(BooleanFn boolean_fn,void* any_data,const Path* is_true,const Path* is_false):boolean_fn(boolean_fn),any_data(any_data),is_true(is_true),is_false(is_false){}
};
using RedirectFn=void(*)(void*);
struct RedirectPath{
  RedirectFn redirect_fn;
  void* any_data;
  const Path* redirect_to;
  RedirectPath(const Path* redirect_to):redirect_fn(nullptr),any_data(nullptr),redirect_to(redirect_to){}
  RedirectPath(const Path* redirect_to,RedirectFn redirect_fn,void* any_data):redirect_fn(redirect_fn),any_data(any_data),redirect_to(redirect_to){}
};
//const Path* const * means that the Path will not be edited nor the paths within the const Path* array.
using RedirectMultiFn=const Path*(*)(void*,const Path*const*);
struct RedirectMultiPath{
  RedirectMultiFn redirect_multi_fn;
  void* any_data;
  const Path*const* paths;
  size_t size;
  RedirectMultiPath(RedirectMultiFn redirect_multi_fn,void* any_data,const Path*const* paths):redirect_multi_fn(redirect_multi_fn),any_data(any_data),paths(paths){}
};
enum class PathType{
  Path,
  Redirect,
  RedirectMulti,
  Boolean,
  Worm,
  WormEnding,
  Ending,
};
struct WormPath{
  std::mt19937* mt;
  const Path* did_not_hide;
  const Path* responded_too_late;
  const Path* hid_but_should_not_hide;
  const Path* success;
  WormPath(std::mt19937* mt,const Path* did_not_hide,const Path* responded_too_late,const Path* hid_but_should_not_hide,const Path* success):mt(mt),did_not_hide(did_not_hide),responded_too_late(responded_too_late),hid_but_should_not_hide(hid_but_should_not_hide),success(success){}
};
union PathProperties{
  PathArray pa;
  RedirectPath rp;
  RedirectMultiPath rmp;
  BooleanPath bp;
  WormPath wp;
  const char* reason;
  //Default constructor used for PathType::Redirect types
  PathProperties(){}
  //The union intializes for each PathType.

  PathProperties(const PathOption* paths,size_t size){ //paths has no information for length, so size determines the length.
    this->pa={paths,size};
  }
  PathProperties(const Path* redirect){
    this->rp=RedirectPath(redirect);
  }
  PathProperties(const Path* redirect,RedirectFn redirect_fn,void* any_data){
    this->rp=RedirectPath(redirect,redirect_fn,any_data);
  }
  PathProperties(RedirectMultiPath rmp){
    this->rmp=rmp;
  }
  PathProperties(BooleanPath bp){
    this->bp=bp;
  }
  PathProperties(WormPath wp){
    this->wp=wp;
  }
  PathProperties(const char* reason){
    this->reason=reason;
  }
};
struct PathOption{
  const char* name;
  const Path* path;
};
//PathType and PathProperties are used to create a tagged-union.
struct Path{
  PathType type;
  const char* words;
  PathProperties properties;
  //Default constructor used for PathType::Redirect types
  Path(){}
  Path(PathType type,const char* words,PathProperties properties):type(type),words(words),properties(properties){}
};
std::vector<const char*>RealHideMessages{
  "You can feel the ground trembling beneath you, its massive form wreaking havoc below the surface. The rumbling is growing loud! Hide!",
  "\"HIDE! YOU FOOL!\" shouted a ghost. \"Take cover! Quickly, find cover before it's too late! Stop reading this! Hide!!!\"",
  "The ceiling splits open as the worm emerges. Get out of sight before it spots you!",
  "A deafening roar fills the cavern, signaling the worm's imminent arrival. Take cover now to evade its deadly jaws!",
  "\"Take cover! Quick! Before the worm comes out. Get into a hiding spot quickly!\" Listen to the ghost!"
};
std::vector<const char*>FakeHideMessages{
  "You can feel the ground trembling beneath you, its massive form wreaking havoc below the surface. The rumbling is shrinking actually! Nevermind!",
  "\"HIDE! YOU FOOL!\" shouted a ghost. \"Take cover! Quickly-\" \"Son, don't confuse the player. Sorry about that! He tricks people into hiding.\"",
  "The ceiling splits open as the worm emerges. Wait a minute. False alarm! That was just debris.",
  "A deafening roar fills the cavern, signaling the worm's imminent arrival. \"Do you like my worm impression?\" said a misleading ghost.",
  "\"Take my covers! Quick! Before they all sell out. Get into a bed sheet cover and be a ghost.\" Who is this ghost trying to fool?"
};
std::vector<const char*>IdleMessages{
  "You pause in the hallway, listening intently for any sign of movement or danger.",
  "You hear the distant rumbling of the worm monster echoing through the hallway, a constant reminder of the danger that lurks nearby.",
  "Chunks of debris litter the hallway, evidence of the worm monster's destructive path.",
  "You can't shake the feeling that you're being watched as you move through the hallway.",
  "The hallway twists and turns unpredictably, its layout seemingly warped by the presence of the worm monster lurking below.",
};
const Path* output_path(const Path* path){
  const Path* new_path=nullptr;
  switch(path->type){
    case PathType::Path:
      while(!new_path){
        if(path->words) std::cout<<path->words<<std::endl;
        size_t path_num=0;
        std::cout<<"Choose the following paths:"<<std::endl;
        for(size_t i=0;i<path->properties.pa.size;i++){
          std::cout<<"\t"<<(i+1)<<") "<<path->properties.pa.paths[i].name<<std::endl;
        }
        std::cout<<">>> ";
        std::cin>>path_num;
        if(path_num<1||path_num>path->properties.pa.size){
          std::cout<<"Invalid path!"<<std::endl;
          continue;
        }
        new_path=path->properties.pa.paths[path_num-1].path;
      }
      break;
    case PathType::Redirect: //This adds words, and then redirects to a new path (And optionally calls a redirect_fn).
      if(path->words) std::cout<<path->words<<std::endl;
      if(path->properties.rp.redirect_fn) path->properties.rp.redirect_fn(path->properties.rp.any_data);
      new_path=path->properties.rp.redirect_to;
      break;
    case PathType::RedirectMulti:
      if(path->words) std::cout<<path->words<<std::endl;
      new_path=path->properties.rmp.redirect_multi_fn(path->properties.rmp.any_data,path->properties.rmp.paths);
      assert(new_path);
      break;
    case PathType::Boolean: //Depending on the boolean_fn and any_data, redirect the path to is_true or is_false.
      if(path->words) std::cout<<path->words<<std::endl;
      if(path->properties.bp.boolean_fn(path->properties.bp.any_data)) new_path=path->properties.bp.is_true;
      else new_path=path->properties.bp.is_false;
      break;
    case PathType::Worm:
      {
        std::discrete_distribution<size_t> dd_prob({70,18,12});
        std::uniform_int_distribution<size_t> uid_idle(0,IdleMessages.size()-1);
        std::uniform_int_distribution<size_t> uid_real_hide(0,RealHideMessages.size()-1);
        std::uniform_int_distribution<size_t> uid_fake_hide(0,FakeHideMessages.size()-1);
        size_t used_idle=IdleMessages.size(); //These are used so that the same number doesn't appear twice for each message type.
        size_t used_real_hide=RealHideMessages.size();
        size_t used_fake_hide=FakeHideMessages.size();
        size_t used_number=0;
        std::string input;
        const size_t Trials=25;
        const auto five_seconds = std::chrono::seconds(5);
        std::mt19937*const mtp=path->properties.wp.mt;
        std::chrono::system_clock::time_point prev;
        std::chrono::system_clock::time_point now;
        for(size_t i=0;i<Trials;i++){
          prev=std::chrono::high_resolution_clock::now();
          std::cout<<"Trial "<<(i+1)<<" out of "<<Trials<<"."<<std::endl;
          switch(dd_prob(*mtp)){
            case 0:
              do used_number=uid_idle(*mtp); while(used_number==used_idle);
              used_idle=used_number;
              std::cout<<IdleMessages[used_number]<<std::endl;
              while(true){
                std::cout<<"To hide: Type 'y'. Otherwise, type 'n'."<<std::endl;
                std::cin>>input;
                if(input=="y"||input=="n") break;
                else std::cout<<"Invalid option!"<<std::endl;
              }
              if(input=="y"){
                new_path=path->properties.wp.hid_but_should_not_hide;
                goto has_failed;
              }
              break;
            case 1:
              do used_number=uid_real_hide(*mtp); while(used_number==used_real_hide);
              used_real_hide=used_number;
              std::cout<<RealHideMessages[used_number]<<std::endl;
              while(true){
                std::cout<<"To hide: Type 'y'. Otherwise, type 'n'."<<std::endl;
                std::cin>>input;
                if(input=="y"||input=="n") break;
                else std::cout<<"Invalid option!"<<std::endl;
              }
              now=std::chrono::high_resolution_clock::now();
              if(prev-now>five_seconds){
                new_path=path->properties.wp.responded_too_late;
                goto has_failed;
              }else if(input=="n"){
                new_path=path->properties.wp.did_not_hide;
                goto has_failed;
              }
              break;
            case 2:
              do used_number=uid_fake_hide(*mtp); while(used_number==used_fake_hide);
              used_fake_hide=used_number;
              std::cout<<FakeHideMessages[used_number]<<std::endl;
              while(true){
                std::cout<<"To hide: Type 'y'. Otherwise, type 'n'."<<std::endl;
                std::cin>>input;
                if(input=="y"||input=="n") break;
                else std::cout<<"Invalid option!"<<std::endl;
              }
              if(input=="y"){
                new_path=path->properties.wp.hid_but_should_not_hide;
                goto has_failed;
              }
              break;
            default:;
          }
          now=std::chrono::high_resolution_clock::now();
          std::cout<<"You have responded in "<<std::chrono::duration_cast<std::chrono::seconds>(now-prev).count()<<" seconds."<<std::endl;
        }
        new_path=path->properties.wp.success;
        has_failed:;
      }
      break;
    case PathType::WormEnding:
    {
      std::ifstream wormfile("worm_monster.txt");
      std::string text;
      while(std::getline(wormfile,text)) std::cout<<text<<std::endl; //Print the worm monster.
      wormfile.close();
      std::cout<<"(Ascii image made using https://www.asciiart.eu/image-to-ascii and http://www.publicdomainfiles.com/show_file.php?id=13927475425048)"<<std::endl;
    }
    [[fallthrough]];
    case PathType::Ending:
      std::cout<<path->words<<std::endl;
      std::cout<<"Game Over!"<<std::endl<<path->properties.reason<<std::endl;
  }
  return new_path;
}
struct DigitCheck{
  size_t* current_digit;
  int digit;
  int* door_combination_p;
  DigitCheck(){};
  DigitCheck(size_t* current_digit,int digit,int* door_combination_p):current_digit(current_digit),digit(digit),door_combination_p(door_combination_p){}
};
bool DigitCheck_fn(void* dc_vp){
  DigitCheck* dc_p=static_cast<DigitCheck*>(dc_vp);
  std::cout<<"You have attempted to enter the number "<<dc_p->digit<<"..."<<std::endl;
  return dc_p->door_combination_p[*dc_p->current_digit]==dc_p->digit; //Is the digit the correct one?
}
struct DragonfireStruct{
  size_t* attempts_left;
  bool is_shield;
  bool use_once; //For shield.
};
bool Dragonfire_fn(void* dfs_vp){
  DragonfireStruct* dfs_p=static_cast<DragonfireStruct*>(dfs_vp);
  std::cout<<"The dragon breathes fire..."<<std::endl;
  if(dfs_p->is_shield){ //The shield option.
    std::cout<<"...However, you attempted to grab the shield..."<<std::endl;
    if(!dfs_p->use_once){
      std::cout<<"...And it temporarily helped against the dragon fire.\nNice! It was a dragon shield. This shield can last for one turn only."<<std::endl;
      *dfs_p->attempts_left+=2;
      dfs_p->use_once=true;
    }else std::cout<<"...But there is no shield here. You have already used the shield."<<std::endl;
  }
  if(*dfs_p->attempts_left>1) std::cout<<"Your protection helped against the dragon fire."<<std::endl;
  else std::cout<<"Oh no! Your protection had ran out!"<<std::endl;
  return --(*dfs_p->attempts_left)==0;
}
struct GetDigitStruct{
  size_t digit_i;
  int* door_combination_p;
  bool is_wrong_digit;
  bool up_or_down_digit;
};
void GetDigit(void* gds_vp){
  GetDigitStruct* gds_p=static_cast<GetDigitStruct*>(gds_vp);
  std::cout<<"You found a paper that describes the ";
  switch(gds_p->digit_i){
    case 0: std::cout<<"first"; break;
    case 1: std::cout<<"second"; break;
    case 2: std::cout<<"third"; break;
    default: std::cout<<"(invalid digit number) ";
  }
  int digit;
  if(gds_p->is_wrong_digit){ //Change the digit one higher or lower.
    digit=gds_p->door_combination_p[gds_p->digit_i];
    if(digit==9) digit--; //Edges are always the opposite.
    else if(digit==0) digit++;
    else{
      if(gds_p->up_or_down_digit) digit--; else digit++;
    }
  }else{
    digit=gds_p->door_combination_p[gds_p->digit_i];
  }
  std::cout<<" digit of the locked door combination. It says "<<digit<<"."<<std::endl;
}
enum class PotionColor{
  Purple,
  Blue,
  Green,
  Yellow,
  Orange,
  Red,
  COUNT, //Only used to count the number of potions.
  NONE,
};
//Macro used to write switch and cases.
#define PCSC(ClassName,Color) case ClassName::Color: return #Color;
const char* GetColor(PotionColor pc){
  switch(pc){
    PCSC(PotionColor,Purple)
    PCSC(PotionColor,Blue)
    PCSC(PotionColor,Green)
    PCSC(PotionColor,Yellow)
    PCSC(PotionColor,Orange)
    PCSC(PotionColor,Red)
    default: return "INVALID COLOR";
  }
}
struct PotionMixNumbers{
  PotionColor pc1,pc2;
  PotionMixNumbers(){}
  PotionMixNumbers(int pc1,int pc2):pc1(static_cast<PotionColor>(pc1)),pc2(static_cast<PotionColor>(pc2)){}
  bool operator==(const PotionMixNumbers&other){ //Used to make sure the potion mixes are unique.
    return this->pc1==other.pc1 && this->pc2==other.pc2;
  }
};
std::ostream& operator<<(std::ostream& os,const PotionMixNumbers& pmn){ //Override operator<< for debugging PotionMixNumbers.
  os<<"{Potion Mix: "<<GetColor(pmn.pc1)<<","<<GetColor(pmn.pc2)<<"}";
  return os;
}
enum class PotionMixName{
  Dragonfire, WizardKnockout, XRay, NONE
};
struct PotionMixStruct{
  PotionColor* pc_mixes;
  PotionColor used_pc;
  size_t* potion_num;
};
void PotionMix_fn(void* pms_vp){
  PotionMixStruct* pms_p=static_cast<PotionMixStruct*>(pms_vp);
  std::cout<<"For the ";
  std::cout<<(*pms_p->potion_num==0?"first":"second");
  std::cout<<" mix, you placed the ";
  std::cout<<GetColor(pms_p->used_pc);
  std::cout<<" potion."<<std::endl;
  pms_p->pc_mixes[*pms_p->potion_num]=pms_p->used_pc; //Place the color.
  (*pms_p->potion_num)++; //Increment for the next mix or end path.
}
struct PotionMixGetPotionStruct{
  bool* potion_inventory;
  PotionColor* pc_mixes;
  PotionMixName(*potion_mix_2d_arr)[static_cast<size_t>(PotionColor::COUNT)]; //Because pointers only decay the first array and potion_mix_2d_arr is a 2d array.
};
struct IncreaseDragonfireProtectionStruct{ //When drinking the Dragonfire potion.
  bool* df_pot_p;
  size_t* dragon_attempts_left;
};
//Macro used to get the array and length of the array.
#define PathArrayAdd(path_options) PathProperties(path_options,sizeof(path_options)/sizeof(PathOption))
int main() {
  //C++ random engine to get digits from 0 to 9 using uid(mt).
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> uid(0,9);
  std::uniform_int_distribution<int> uid_wrong_digit(0,2); //Used to give one wrong digit.
  std::uniform_int_distribution<int> uid_bool(0,1); //To make a wrong digit higher or lower.
  std::uniform_int_distribution<int> uid_potion_mix(0,static_cast<int>(PotionColor::COUNT)-1); //To make potions in the F_path.
  //Used temporarily as C++ doesn't allow for zero-sized arrays.
  Path PlaceHolder(PathType::Ending,"TODO Add Words",PathProperties("TODO Ending Here"));
  while(true){
    
    //Game logic to create random potions or lock combinations.
    int door_combination[3]={uid(mt),uid(mt),uid(mt)}; //Create the code for every game.
    GetDigitStruct d1{0,door_combination,false,static_cast<bool>(uid_bool(mt))},
      d2{1,door_combination,false,static_cast<bool>(uid_bool(mt))},
      d3{2,door_combination,false,static_cast<bool>(uid_bool(mt))};
    switch(uid_wrong_digit(mt)){ //Only one digit will randomly give the wrong digit.
      case 0: d1.is_wrong_digit=true; break;
      case 1: d2.is_wrong_digit=true; break;
      case 2: d3.is_wrong_digit=true; break;
    }
    PotionMixName potion_mix_2d_arr[static_cast<int>(PotionColor::COUNT)][static_cast<int>(PotionColor::COUNT)];
    for(size_t i=0;i<static_cast<int>(PotionColor::COUNT);i++){
      for(size_t j=0;j<static_cast<int>(PotionColor::COUNT);j++){
        potion_mix_2d_arr[i][j]=PotionMixName::NONE; //Just set to none.
      }
    }
    PotionMixNumbers dragon_fire;
    PotionMixNumbers wizard_knockout;
    PotionMixNumbers xray;
    dragon_fire=PotionMixNumbers(uid_potion_mix(mt),uid_potion_mix(mt));
    //do while because Combinations must be unique for all potions.
    do wizard_knockout=PotionMixNumbers(uid_potion_mix(mt),uid_potion_mix(mt));
    while (dragon_fire==wizard_knockout);
    do xray=PotionMixNumbers(uid_potion_mix(mt),uid_potion_mix(mt));
    while (dragon_fire==xray||wizard_knockout==xray);
    //Mark the below potion mixes as not deadly and explosive.
    potion_mix_2d_arr[static_cast<int>(dragon_fire.pc1)][static_cast<int>(dragon_fire.pc2)]=PotionMixName::Dragonfire;
    potion_mix_2d_arr[static_cast<int>(wizard_knockout.pc1)][static_cast<int>(wizard_knockout.pc2)]=PotionMixName::WizardKnockout;
    potion_mix_2d_arr[static_cast<int>(xray.pc1)][static_cast<int>(xray.pc2)]=PotionMixName::XRay;
    bool potion_inventory[3]={false,false,false}; //For Dragonfire, Wizardknockout, and Xray respectively.
    //Commented for debugging purposes.
    //std::cout<<d1.is_wrong_digit<<d2.is_wrong_digit<<d3.is_wrong_digit<<std::endl;
    //std::cout<<door_combination[0]<<door_combination[1]<<door_combination[2]<<std::endl;
    //std::cout<<"Dragonfire is "<<dragon_fire<<", Wizard Knockout is "<<wizard_knockout<<", XRay is "<<xray<<std::endl;
    
    //Gold_path
    Path C_path;
    Path Gold_path;
    bool unlocked_gold_door=false;
    bool wizard_knocked_out=false;
    Path Gold_good_ending(PathType::Ending,"Now that the wizard has been knocked out, you fill your backpack with as much treasures as you can carry.\nCongratulations! You have beaten the Golden Wizard's traps and successfully stolen his treasures!\nYou lived the rest of your life old and rich.",PathProperties("The (only) Good Ending"));
    Path Gold_bad_ending(PathType::Ending,"As soon as you touch the gold, you noticed that your fingers start to turn into gold.\nThe magic seems to rapidly go through your arm and then your entire body. Oh no! You're turning into a gold statue.\n\"YOU FOOL! YOU THOUGHT YOU COULD GET MY TREASURES EASILY! HAHAHAHA!\nYou will make a fine statue to ward off other fools that steal my treasures.\"",PathProperties("Priceless Gold Trap Ending: One of Golden Wizard's favorite traps."));
    Path Gold_grab_boolean(PathType::Boolean,"You attempted to grab the gold...",PathProperties(BooleanPath(
      [](void* wizard_knocked_out_vp){
        return *static_cast<bool*>(wizard_knocked_out_vp);
      },&wizard_knocked_out,&Gold_good_ending,&Gold_bad_ending
    )));
    Path Gold_ghost_path(PathType::Redirect,"The ghost explains to you \"The wizard has rigged the treasure where if you touch the treasure. You will turn gold yourself.\nAs long as he is awake, the magic will turn you into a golden statue.\"\nTurning into a statue would be a horrible ending for this adventure. Let's not do that.\nHe further explains that you can try to knock a wizard out cold with a Wizard Knockout potion.",
      PathProperties(&Gold_path,[](void* df_potion_vp){
        PotionMixNumbers* df_potion_p=static_cast<PotionMixNumbers*>(df_potion_vp);
        std::cout<<"He tells you that the potion combination is ";
        std::cout<<GetColor(df_potion_p->pc1)<<" and "<<GetColor(df_potion_p->pc2);
        std::cout<<". You now know how to create the wizard knockout potion!"<<std::endl;
        std::cout<<"\"Once you get the potion. Try to get the Wizard, and throw the potion at him.\nHe will be knocked unconscious, and the treasure will be yours.\""<<std::endl;
      },&wizard_knockout)
    );
    PathOption Gold_path_options[]={
      {"Grab the gold",&Gold_grab_boolean},
      {"Talk to the ghost",&Gold_ghost_path},
      {"Go Back",&C_path},
    };
    Gold_path=Path(PathType::Path,"You see piles of gold and treasures throughout this room. Just as you were to stuff your pockets, a ghost has appeared and said \"NO DON'T!\"",PathArrayAdd(Gold_path_options));
    Path D_to_Gold_path(PathType::Redirect,"You have finally unlocked the Golden Wizard's golden door. Woohoo!",PathProperties(&Gold_path
    ,[](void* ugd_vp){
      *static_cast<bool*>(ugd_vp)=true;
    },&unlocked_gold_door));
    //G_path
    Path B_path;
    Path G_path_ending;
    Path G_ending_to_C(PathType::Redirect,"You went back to the entrance of the Golden Wizard's tower, having knowledge of this shortcut to go back to this room if you wish.",PathProperties(&C_path));
    bool shortcut=false;
    Path G_paper1(PathType::Redirect,nullptr,PathProperties(&G_path_ending,GetDigit,&d2));
    Path G_paper2(PathType::Redirect,nullptr,PathProperties(
      &G_path_ending,[](void* df_potion_vp){
        PotionMixNumbers* df_potion_p=static_cast<PotionMixNumbers*>(df_potion_vp);
        std::cout<<"This paper seems to describe how to make the Dragonfire potion.\nThe potion combination is ";
        std::cout<<GetColor(df_potion_p->pc1)<<" and "<<GetColor(df_potion_p->pc2);
        std::cout<<". You now know how to create the dragonfire potion!"<<std::endl;
      },&dragon_fire
    ));
    PathOption G_path_ending_options[]={
      {"See Paper 1",&G_paper1},
      {"See Paper 2",&G_paper2},
      {"Go Back to Shortcut",&G_ending_to_C},
    };
    G_path_ending=Path(PathType::Path,"You see two papers near the hallway.",PathArrayAdd(G_path_ending_options));
    Path G_path_ending_shortcut(PathType::Redirect,"After wandering through the endless hallways, you have seemed to reached the end.\nYou have noticed the shortcut, so that you don't have to wander in the hallways again with the monster.\nHorray!"
      ,PathProperties(&G_path_ending,
        [](void* shortcut_vp){
          *static_cast<bool*>(shortcut_vp)=true;
        },&shortcut
      )
    );
    Path G_path_worm_meal1(PathType::WormEnding,"Oh no! You didn't hide when you should have. The worm monster has spotted you, and devoured you due to your mistake!",PathProperties("Worm Meal Ending. Make sure to read the messages carefully."));
    Path G_path_worm_meal2(PathType::WormEnding,"Oh no! You have responded in more than 5 seconds. The worm monster has spotted you, and devoured you due to hesitation!",PathProperties("Worm Meal Ending. Make sure to read the messages quickly!"));
    Path G_path_worm_meal3(PathType::WormEnding,"Oh no! You hid when you shouldn't have hidden! The worm monster has detected you inside a hiding spot and devoured you alive.",PathProperties("Worm Meal Ending. Be careful of fake messages that sound like that you should hide."));
    Path G_path_worm(PathType::Worm,nullptr,PathProperties(WormPath(&mt,&G_path_worm_meal1,&G_path_worm_meal2,&G_path_worm_meal3,&G_path_ending_shortcut)));
    PathOption G_path_options[]={
      {"Proceed",&G_path_worm},
      {"Not Yet (Exit Basement)",&C_path},
    };
    Path G_path(PathType::Path,"\nIn this minigame, to hide, type 'y'. To not hide, type 'n'.\nYou have 5 seconds until the worm monster spots you and eats you alive.\nIt will also eat you if you don't have to hide.\nYou have to survive 25 trials.\nWill you procede or not?",PathArrayAdd(G_path_options));
    Path C_to_G_path(PathType::Boolean,"You went to the basement to what looks like endless and endless hallways.\nA ghost appeared and said to you \"Once you reach the end, it will help on your journey to the Golden Wizard's treasures.\nHowever, the worm monster guards this hallway. If you see or hear anything strange, hide!\nDon't hide if nothing unusual happens, or the worm monster will also consume you!\nGhosts may tell you to hide, but some may trick you.",PathProperties(BooleanPath(
      [](void* shortcut_vp){
        bool found_shortcut=*static_cast<bool*>(shortcut_vp);
        if(found_shortcut){
          std::cout<<"Because you went to the end the first time, you remembered the shortcut to the end of this maze and went there."<<std::endl;
        }
        return found_shortcut;
      },&shortcut,&G_path_ending,&G_path
    )));
    //F_path
    PotionColor pc_mixes[2];
    Path F_path;
    Path F_mix_redir_multi;
    size_t potion_num=0;
    PotionMixGetPotionStruct gmgps{potion_inventory,pc_mixes,potion_mix_2d_arr};
    Path F_mix_success(PathType::Redirect,nullptr,PathProperties(&F_path,[](void* pmgps_vp){
      PotionMixGetPotionStruct* pmgps_p=static_cast<PotionMixGetPotionStruct*>(pmgps_vp);
      PotionMixName potion=pmgps_p->potion_mix_2d_arr[static_cast<size_t>(pmgps_p->pc_mixes[0])][static_cast<size_t>(pmgps_p->pc_mixes[1])];
      std::cout<<"...there was no explosion. Phew! You have successfully created the ";
      switch(potion){
        case PotionMixName::Dragonfire: std::cout<<"Dragonfire"; break;
        case PotionMixName::WizardKnockout: std::cout<<"Wizard Knockout"; break;
        case PotionMixName::XRay: std::cout<<"XRay"; break;
        default: std::cout<<"INVALID POTION";
      }
      std::cout<<" Potion!"<<std::endl;
      pmgps_p->potion_inventory[static_cast<size_t>(potion)]=true; //Activate as true.
      switch(potion){
        case PotionMixName::Dragonfire:
          std::cout<<"You have drank it. This helps protect against dragon fire for one burst."<<std::endl;
          break;
        case PotionMixName::WizardKnockout:
          std::cout<<"This helps to incapacitate wizards. But because you're not a wizard, you store it in your inventory.\nPerhaps you can use this to knock down the Golden Wizard."<<std::endl;
          break;
        case PotionMixName::XRay:
          std::cout<<"You have drank it. This helps look for things inside walls."<<std::endl;
          break;
        default:;
      }
    },&gmgps
    ));
    Path F_mix_failure(PathType::Ending,"...it created a big explosion. Yikes! That was a bad combination to use!\nYou and the whole room evaporated in a disasterous explosion.",PathProperties("Explosion ending. Potion mixing is a dangerous endeavor!"));
    Path F_mix_is_success(PathType::Boolean,nullptr,PathProperties(BooleanPath([](void* pmgps_vp){
      PotionMixGetPotionStruct* pmgps_p=static_cast<PotionMixGetPotionStruct*>(pmgps_vp);
      std::cout<<"As soon as you mixed the "<<GetColor(pmgps_p->pc_mixes[0])<<" potion with the "<<GetColor(pmgps_p->pc_mixes[0])<<" potion..."<<std::endl;
      return pmgps_p->potion_mix_2d_arr[static_cast<size_t>(pmgps_p->pc_mixes[0])][static_cast<size_t>(pmgps_p->pc_mixes[1])]!=PotionMixName::NONE;
    },&gmgps,&F_mix_success,&F_mix_failure)));
    PotionMixStruct pms_arr[6];
    Path F_mixing[6];
    for(size_t i=0;i<static_cast<size_t>(PotionColor::COUNT);i++){ //Initialize all colors and PotionMixStructs in order.
      pms_arr[i]=PotionMixStruct{pc_mixes,static_cast<PotionColor>(i),&potion_num};
      F_mixing[i]=Path(PathType::Redirect,nullptr,PathProperties(&F_mix_redir_multi,PotionMix_fn,&pms_arr[i]));
    }
    PathOption F_mix_options[]={
      {"Purple",&F_mixing[0]},
      {"Blue",&F_mixing[1]},
      {"Green",&F_mixing[2]},
      {"Yellow",&F_mixing[3]},
      {"Orange",&F_mixing[4]},
      {"Red",&F_mixing[5]},
      {"Cancel",&F_path},
    };
    Path F_mix2(PathType::Path,"You see 6 colors of potions and a cauldron to mix them.\nYou need two potion colors to mix into a potion.\nYou can cancel if you make a mistake. Choose the first following color to mix.",PathArrayAdd(F_mix_options));
    Path F_mix(PathType::Path,"You see 6 colors of potions and a cauldron to mix them.\nYou need two potion colors to mix into a potion.\nYou can cancel if you make a mistake. Choose the first following color to mix.",PathArrayAdd(F_mix_options));
    const Path* F_mix_paths[]={
      &F_mix,
      &F_mix2,
      &F_mix_is_success,
    };
    F_mix_redir_multi=Path(PathType::RedirectMulti,nullptr,PathProperties(RedirectMultiPath(
      [](void* potion_num_vp,const Path*const* paths){
        size_t* potion_num_p=static_cast<size_t*>(potion_num_vp);
        return paths[*potion_num_p];
      },&potion_num,F_mix_paths
    )));
    Path F_path_to_mix(PathType::Redirect,"You want to mix potions. However, the ghost says \"Be careful! One wrong mix, and the whole room can explode\".\nOh dear! Perhaps there's clues to create potions correctly.",PathProperties(&F_mix_redir_multi,
      [](void* potion_num_vp){
        *static_cast<size_t*>(potion_num_vp)=0; //Reset counter to first mix.
      },&potion_num
    ));
    Path F_ghost(PathType::Redirect,"The ghost speaks to you. It says \"Be warned! One of the digits of the locked door that you see is wrong!\nIt's one number higher or lower than it should be.\"\nThat seems to be useful information.",PathProperties(&F_path));
    PathOption F_options[]={
      {"Check ghost",&F_ghost},
      {"Mix potions",&F_path_to_mix},
      {"Go back",&C_path},
    };
    F_path=Path(PathType::Path,"Maybe you can create potions in this room.",PathArrayAdd(F_options));
    Path C_to_F_path(PathType::Redirect,"When you reached the door on the right, there seems to be various incredients, a cauldron, and a ghost in this room.",PathProperties(&F_path));
    //E_path
    Path E_path_remember1;
    size_t dragon_attempts_left=1;
    bool seen_papers[2]={false,false}; //Because you only see the papers once, this is to make the character remember the numbers and potion combinations.
    Path E_paper1b(PathType::Redirect,nullptr,PathProperties(&E_path_remember1,GetDigit,&d1));
    RedirectFn E_paper2d_lambda=[](void* xray_potion_vp){
      PotionMixNumbers* xray_potion_p=static_cast<PotionMixNumbers*>(xray_potion_vp);
      std::cout<<"This paper seems to describe how to make the X-ray potion.\nThe potion combination is ";
      std::cout<<GetColor(xray_potion_p->pc1)<<" and "<<GetColor(xray_potion_p->pc2);
      std::cout<<". You now know how to create the X-ray potion!"<<std::endl;
    };
    Path E_paper2b(PathType::Redirect,nullptr,PathProperties(&E_path_remember1,E_paper2d_lambda,&xray));
    Path E_paper1a(PathType::Redirect,nullptr,PathProperties(&E_paper1b,[](void* seen_paper_vp){
      *static_cast<bool*>(seen_paper_vp)=true;
    },&seen_papers[0]));
    Path E_paper2a(PathType::Redirect,nullptr,PathProperties(&E_paper2b,[](void* seen_paper_vp){
      *static_cast<bool*>(seen_paper_vp)=true;
    },&seen_papers[1]));
    DragonfireStruct dfs_papers{&dragon_attempts_left,false,false};
    DragonfireStruct dfs_shield{&dragon_attempts_left,true,false};
    Path E_death_by_df(PathType::Ending,"Yeowch! You were burnt by the dragon fire.\nYou had no protection left!",PathProperties("Burnt Ending. Dragon fire is hot! It's also deadly!"));
    Path E_df_to_paper1(PathType::Boolean,nullptr,PathProperties(BooleanPath(Dragonfire_fn,&dfs_papers,&E_death_by_df,&E_paper1a)));
    Path E_df_to_paper2(PathType::Boolean,nullptr,PathProperties(BooleanPath(Dragonfire_fn,&dfs_papers,&E_death_by_df,&E_paper2a)));
    Path E_df_to_shield(PathType::Boolean,nullptr,PathProperties(BooleanPath(Dragonfire_fn,&dfs_shield,&E_death_by_df,&E_path_remember1)));
    PathOption E_options[]={
      {"See Paper 1",&E_df_to_paper1},
      {"See Paper 2",&E_df_to_paper2},
      {"Get Shield",&E_df_to_shield},
      {"Go back",&C_path},
    };
    Path E_path(PathType::Path,"Once you look the papers, you will not forget, as the words will be burned in your memory in this place, or something.",PathArrayAdd(E_options));
    Path E_paper2b_remember(PathType::Redirect,nullptr,PathProperties(&E_path,E_paper2d_lambda,&xray));
    Path E_path_remember2(PathType::Boolean,"You attempt to remember the second paper...",
      PathProperties(BooleanPath([](void* seen_paper_vp){
        bool seen_paper=*static_cast<bool*>(seen_paper_vp);
        if(!seen_paper) std::cout<<"But you didn't see it yet..."<<std::endl;
        return seen_paper;
        },&seen_papers[1],&E_paper2b_remember,&E_path
    )));
    Path E_paper1b_remember(PathType::Redirect,nullptr,PathProperties(&E_path_remember2,GetDigit,&d1));
    E_path_remember1=Path(PathType::Boolean,"Because of the horrible experiences with Dragonfire, you attempt to remember the first paper...",
      PathProperties(BooleanPath([](void* seen_paper_vp){
        bool seen_paper=*static_cast<bool*>(seen_paper_vp);
        if(!seen_paper) std::cout<<"But you didn't see it yet..."<<std::endl;
        return seen_paper;
        },&seen_papers[0],&E_paper1b_remember,&E_path_remember2
    )));
    IncreaseDragonfireProtectionStruct idps{&potion_inventory[0],&dragon_attempts_left};
    Path C_to_E_path(PathType::Redirect,"You have entered the left path. It feels very hot and smoky in here.\nUhoh! This corridor houses a live dragon. And he doesn't seem very happy.\nWhen you went to dragon school, you remembered that if you have\na dragon shield or a dragonfire potion, you can temporarily be immune from their fire for one turn.",
      PathProperties(&E_path_remember1,[](void* idps_vp){
        IncreaseDragonfireProtectionStruct* idps_p=static_cast<IncreaseDragonfireProtectionStruct*>(idps_vp);
        if(*idps_p->df_pot_p){
          std::cout<<"Because you have drank a dragonfire potion, your protection from dragon fire has increased."<<std::endl;
          (*idps_p->dragon_attempts_left)++;
          *idps_p->df_pot_p=false;
        }
      },&idps
    ));
    //D_path
    size_t current_digit=0;
    size_t incorrect_digits=0;
    Path D_redir_multi;
    Path D_correct_number(PathType::Redirect,"The digit was correct!",PathProperties(&D_redir_multi,
      [](void* cd_vp){
        (*static_cast<size_t*>(cd_vp))++;
      }
    ,&current_digit));
    Path D_too_many_incorrects(PathType::Ending,"Just as you were attempting to open the door to the treasures, loud bells have rung in your location.\nThe Golden Wizard as shouted \"AHA! THIEF!\"\nThe police seem to be out the front door.\nOh dear! It seems the Golden Wizard has called the police, because of his security system\nof entering too many incorrect attempts. You are now arrested for trespassing.",PathProperties("Jail Ending. Perhaps you shouldn't have guessed the code. Maybe there's clues around other paths."));
    Path D_incorrect_number(PathType::Boolean,"The digit was incorrect!",PathProperties(BooleanPath([](void* id_vp){
      size_t* id_p=static_cast<size_t*>(id_vp);
      return ++(*id_p)==3; //3rd attempt is the Jail Ending.
    },&incorrect_digits,&D_too_many_incorrects,&D_redir_multi)));
    DigitCheck dc_digits[10];
    BooleanPath bp_digits[10];
    Path digits_paths[10];
    for(size_t i=0;i<10;i++){ //This code checks against the current door_combination number (Indexed using current_digit).
      dc_digits[i]=DigitCheck(&current_digit,static_cast<int>(i),door_combination);
      bp_digits[i]=BooleanPath(DigitCheck_fn,&dc_digits[i],&D_correct_number,&D_incorrect_number);
      digits_paths[i]=Path(PathType::Boolean,nullptr,PathProperties(bp_digits[i]));
    }
    PathOption D_options[]={
      {"1",&digits_paths[1]},
      {"2",&digits_paths[2]},
      {"3",&digits_paths[3]},
      {"4",&digits_paths[4]},
      {"5",&digits_paths[5]},
      {"6",&digits_paths[6]},
      {"7",&digits_paths[7]},
      {"8",&digits_paths[8]},
      {"9",&digits_paths[9]},
      {"0",&digits_paths[0]},
      {"Exit",&C_path}
    };
    Path D_path_multi[3]={
      Path(PathType::Path,"What do you think the first digit is?",PathArrayAdd(D_options)),
      Path(PathType::Path,"What do you think the second digit is?",PathArrayAdd(D_options)),
      Path(PathType::Path,"What do you think the third digit is?",PathArrayAdd(D_options)),
    };
    const Path* D_path_multi_p[4]={
      &D_path_multi[0],&D_path_multi[1],&D_path_multi[2],&D_to_Gold_path
    };
    D_redir_multi=Path(PathType::RedirectMulti,nullptr,PathProperties(RedirectMultiPath(
      [](void* current_digit_vp,const Path*const* paths){
        size_t* current_digit_p=static_cast<size_t*>(current_digit_vp);
        return paths[*current_digit_p];
      },&current_digit,D_path_multi_p
    )));
    Path D_XRay_paper(PathType::Redirect,nullptr,PathProperties(&D_redir_multi,GetDigit,&d3));
    Path D_HasXRay(PathType::Boolean,nullptr,PathProperties(BooleanPath([](void* xr_pot_vp){
      bool* xr_pot_p=static_cast<bool*>(xr_pot_vp);
      if(*xr_pot_p) std::cout<<"Because you have drank the XRay potion, you now see a paper in the walls."<<std::endl;
      return *xr_pot_p;
    },&potion_inventory[2],&D_XRay_paper,&D_redir_multi)));
    Path C_to_D_path(PathType::Redirect,"You have approached a locked golden door with a combination of 3 digits.\nPerhaps this is where the Golden Wizard's treasure lies.\nWill you attempt to unlock the door?",PathProperties(&D_HasXRay));
    Path C_to_D_or_Gold_path(PathType::Boolean,nullptr,PathProperties(BooleanPath(
      [](void* ugd_vp){return *static_cast<bool*>(ugd_vp); },&unlocked_gold_door,&Gold_path,&C_to_D_path
    )));
    //C_path
    PathOption C_options[]={
      {"Left Door",&C_to_E_path},
      {"Middle Door",&C_to_D_or_Gold_path},
      {"Right Door",&C_to_F_path},
      {"Basement",&C_to_G_path},
      {"Go back",&B_path},
    };
    C_path=Path(PathType::Path,"You are now at the tower's front entrance with 4 paths.",PathArrayAdd(C_options));
    Path B_to_C_path(PathType::Redirect,"...and you have successfully opened the door and went inside.",PathProperties(&C_path));
    //B_path
    bool B_key=false;
    Path B_open_no(PathType::Redirect,"...but it seems to be locked.",PathProperties(&B_path));
    Path B_open(PathType::Boolean,"You tried to open the door...",PathProperties(BooleanPath(
      [](void* B_key_vp){
        return *static_cast<bool*>(B_key_vp);
      }, &B_key, &B_to_C_path, &B_open_no
    )));
    Path B_search_no_key(PathType::Redirect,"...but there seems to be nothing there.",PathProperties(&B_path));
    Path B_search_key(PathType::Redirect,"...and you have successfully found a key.",PathProperties(&B_path));
    Path B_search(PathType::Boolean,"You searched around to see if there's anything to open the door...",PathProperties(BooleanPath(
      [](void* B_key_vp){
        bool* B_key_p=static_cast<bool*>(B_key_vp);
        if(!*B_key_p){
          *B_key_p=true;
          return true;
        }else return false;
      }, &B_key, &B_search_key, &B_search_no_key
    )));
    Path B_ring_with_pot(PathType::Redirect,"As you rang the doorbell, the Wizard immediately spawns in front of you.\nBefore he casts his spell, you throw the wizard knockout potion.\nIn an instant, he fell asleep. \"NOOOooooo! zzz...\"",
      PathProperties(&B_path,[](void* wizard_knocked_out_vp){
        *static_cast<bool*>(wizard_knocked_out_vp)=true;
      },&wizard_knocked_out)
    );
    Path B_ring_no_pot(PathType::Ending,"As you rang the doorbell, the Wizard immediately spawns in front of you. You were blinded by the Wizard's spell.\nYou heard a loud static noise from the spell.\nA bright light and noise has consumed you.",PathProperties("Dead Ending. Guess the Gold Wizard didn't want visitors."));
    Path B_ring_boolean2(PathType::Boolean,nullptr,PathProperties(
      BooleanPath([](void* wizard_knockout_vp){ return *static_cast<bool*>(wizard_knockout_vp); },&potion_inventory[1],&B_ring_with_pot,&B_ring_no_pot)
    ));
    PathOption B_ring_q_options[]={
      {"Yes",&B_ring_boolean2},
      {"No",&B_path},
    };
    Path B_ring_q(PathType::Path,"Are you sure you want to ring the doorbell? That GET OUT sign was intimidating after all.",PathArrayAdd(B_ring_q_options));
    Path B_ring_but_wizard_already_knocked_out(PathType::Redirect,"There's no need to ring the doorbell anymore. You have already knocked out the wizard.",PathProperties(&B_path));
    Path B_ring_boolean1(PathType::Boolean,nullptr,PathProperties(
      BooleanPath([](void* wizard_knocked_out_vp){ return *static_cast<bool*>(wizard_knocked_out_vp); },&wizard_knocked_out,&B_ring_but_wizard_already_knocked_out,&B_ring_q)
    ));
    Path B_leave(PathType::Ending,"A door has defeated you.",PathProperties("Left Adventure Ending. Locked doors are hard to open."));
    PathOption B_leave_q_options[]={
      {"Yes",&B_leave},
      {"No",&B_path},
    };
    Path B_leave_q(PathType::Path,"Are you sure you want to leave? Your adventure ends after saying yes.",PathArrayAdd(B_leave_q_options));
    PathOption B_options[]={
      {"Open the door",&B_open},
      {"Ring the doorbell",&B_ring_boolean1},
      {"Search",&B_search},
      {"Leave",&B_leave_q},
    };
    B_path=Path(PathType::Path,"You entered the Gold Wizard's Tower to search for treasures.\nYou are near the front door entrance.\nThe sign says \"GET OUT OR PERISH!\" That's not a good sign.",PathArrayAdd(B_options));
    //A_path
    Path A_no(PathType::Ending,"Ok goodbye...",PathProperties("Chicken Ending. You didn't want to start the adventure.\nWas it because of the dangers and the adventurers never returning?"));
    PathOption A_options[]={
      {"Yes. Let's go forth!",&B_path},
      {"No. Too dangerous for me!",&A_no},
    };
    Path A_path(PathType::Path,"You are an adventurer that wanted to find the treasures of the Gold Wizard.\nHe is an old, eccentric hostile man. There are tons of adventurers who were willing to embark the adventure. None of them have ever returned.\nYou are aware of the dangers that lurk ahead, and you might die in the adventure.\nAre you willing to proceed?",PathArrayAdd(A_options));
    //Start the program with the A_path.
    const Path* current_path=&A_path;
    while((current_path=output_path(current_path))){}
    while(true){
      std::cout<<std::endl<<"Play again? Type 'y' to play again or 'n' to exit."<<std::endl;
      char play_again_c;
      std::cin>>play_again_c;
      if(play_again_c=='n') goto done;
      else if(play_again_c=='y') break;
    }
  }
  done:;
}