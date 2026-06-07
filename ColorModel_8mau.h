#pragma once
#include <stdarg.h>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class DecisionTree {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        if (x[0] <= 0.6111111044883728) {
                            if (x[2] <= 0.30476053059101105) {
                                if (x[1] <= 0.47985348105430603) {
                                    return 7;
                                }

                                else {
                                    return 2;
                                }
                            }

                            else {
                                if (x[0] <= 0.2000138908624649) {
                                    if (x[0] <= 0.1867079883813858) {
                                        return 1;
                                    }

                                    else {
                                        return 1;
                                    }
                                }

                                else {
                                    if (x[1] <= 0.29351939260959625) {
                                        if (x[0] <= 0.34015922248363495) {
                                            return 4;
                                        }

                                        else {
                                            return 3;
                                        }
                                    }

                                    else {
                                        if (x[2] <= 0.3449544757604599) {
                                            if (x[1] <= 0.323627233505249) {
                                                if (x[2] <= 0.33946840465068817) {
                                                    return 6;
                                                }

                                                else {
                                                    return 0;
                                                }
                                            }

                                            else {
                                                if (x[3] <= 0.9620915055274963) {
                                                    return 6;
                                                }

                                                else {
                                                    return 6;
                                                }
                                            }
                                        }

                                        else {
                                            if (x[1] <= 0.3217143416404724) {
                                                if (x[2] <= 0.3477548807859421) {
                                                    return 0;
                                                }

                                                else {
                                                    return 0;
                                                }
                                            }

                                            else {
                                                if (x[1] <= 0.32203373312950134) {
                                                    return 6;
                                                }

                                                else {
                                                    return 0;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        else {
                            return 5;
                        }
                    }

                protected:
                };
            }
        }
    }