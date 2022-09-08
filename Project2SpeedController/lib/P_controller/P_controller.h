class P_controller{
  private:
  double k_p=1000;
  public:
    P_controller(double K_p);  
    double update(double ref, double actual);
};
