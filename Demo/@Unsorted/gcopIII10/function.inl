
		case 1: X[0] = (X[1] - 2.0)*(X[1] - 2.0) +
						(X[2] -1.0)*(X[2] - 1.0);
				break;

		case 2:
				X[0] = X[1]*X[1] + X[2]*X[2];
				break;

		case 3:
				X[0] = (1.0 - X[1])*(1.0 - X[1]);
				break;

		case 22:
			X[0] = 2 * X[1] * X[1] + 4 * X[2] * X[2] - 2 * X[1] * X[2] - 3 * X[1] + 6 * X[2];
			break;


