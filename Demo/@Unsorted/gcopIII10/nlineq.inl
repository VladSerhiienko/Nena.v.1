
	case 1:
		nlineq[1] = -1 * X[1] * X[1] + X[2];
		break;

	case 2:
		nlineq[1] = X[1] * X[1] + X[2] * X[2] - 1.0;
		nlineq[2] = 9 * X[1] * X[1] + X[2] * X[2] - 9.0;
		nlineq[3] = X[1] * X[1] - X[2];
		nlineq[4] = X[2] * X[2] - 1.0;
		break;

	case 22:
		//printf("nlineq22\n");
		//nlineq[1] = X[1];
		//nlineq[2] = -1.73205080 * X[1] * X[1] + X[2];
		//nlineq[3] = 16 - 2 * X[1] * X[1] - X[2] * X[2];
		nlineq[1] = -1.73205080 * X[1] * X[1] + X[2];
		nlineq[2] = 16 - 2 * X[1] * X[1] - X[2] * X[2];
		nlineq[3] = X[1];
		break;


