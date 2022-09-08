class P_controller{
  private:
  double k_p=1000;
  public:
    double update(double ref, double actual);
};
