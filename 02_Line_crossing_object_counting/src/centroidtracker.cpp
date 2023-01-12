#include "centroidtracker.h"
#include <iterator>

using namespace std;

CentroidTracker::CentroidTracker(int maxDisappear, int maxDist) {
    this->nextObjID = 0;
    this->maxDisappear = maxDisappear;
    this->maxDist = maxDist;
}

/*****************************************
* Function Name : calcDistance
* Description   : Returns the euclideans distance
* Arguments     : 4 co-ordinates (x1,y1,x2,y2)
* Return value  : distance
******************************************/

double CentroidTracker::calcDistance(double xmin, double ymin, double xmax, double ymax) {
    double x = xmin - xmax;
    double y = ymin - ymax;
    double dist = sqrt((x * x) + (y * y));      /* calculating Euclidean distance */
    return dist;
}

/*****************************************
* Function Name : register_Object
* Description   : registers the new objectid in the object
* Arguments     : centroid points (x,y)
* Return value  : -
******************************************/

void CentroidTracker::register_Object(int centerX, int centerY) {
    int object_ID = this->nextObjID;
    this->objects.push_back({object_ID, {centerX, centerY}});
    this->disappeared.insert({object_ID, 0});
    this->nextObjID += 1;
}

/*****************************************
* Function Name : findMin
* Description   : Helper function to find the min
* Arguments     : vector amd position
* Return value  : minimum
******************************************/

vector<float>::size_type findMin(const vector<float> &vec, vector<float>::size_type pos = 0) {
    if (vec.size() <= pos) return (vec.size());
    vector<float>::size_type min = pos;
    for (vector<float>::size_type i = pos + 1; i < vec.size(); i++) {
        if (vec[i] < vec[min]) min = i;
    }
    return (min);
}

/*****************************************
* Function Name : update
* Description   : updates the tracker object after every detection
* Arguments     : boxes detection boxres
* Return value  : objects(current running trackers)
******************************************/

vector<pair<int, pair<int, int>>> CentroidTracker::update(vector<vector<int>> boxes) {
    if (boxes.empty()) {
        auto it = this->disappeared.begin();
        while (it != this->disappeared.end()) {
            it->second++;
            if (it->second > this->maxDisappear) {
                this->objects.erase(remove_if(this->objects.begin(), this->objects.end(), [it](auto &elem) {
                    return elem.first == it->first;
                }), this->objects.end());

                this->path_keeper.erase(it->first);
                it = this->disappeared.erase(it);
            } 
            else {
                ++it;
            }
        }
        return this->objects;
    }

    /* initialize an array of input centroids for the current frame */
    vector<pair<int, int>> inputCentroids;
    for (auto box : boxes) {
        int centerX = int((box[0] + box[2]) / 2.0);
        int centerY = int((box[1] + box[3]) / 2.0);
        if (centerX < 10 || centerY < 10) continue;
        inputCentroids.push_back(make_pair(centerX, centerY));
    }

    /* if we are currently not tracking any objects take the input centroids and register each of them */
    if (this->objects.empty()) {
        for (auto ip_cent: inputCentroids) {
            this->register_Object(ip_cent.first, ip_cent.second);
        }
    }
    /* otherwise, there are currently tracking objects so we need to try to match the
       input centroids to existing object centroids */
    else {
        vector<int> objectIDs;
        vector<pair<int, int>> objectCentroids;
        for (auto object : this->objects) {
            objectIDs.push_back(object.first);
            objectCentroids.push_back(make_pair(object.second.first, object.second.second));
        }

        /* Calculate Distances */
        vector<vector<float>> Distances;
        for (int i = 0; i < objectCentroids.size(); ++i) {
            vector<float> temp_D;
            for (vector<vector<int>>::size_type j = 0; j < inputCentroids.size(); ++j) {
                double dist = calcDistance(objectCentroids[i].first, objectCentroids[i].second, inputCentroids[j].first,
                                           inputCentroids[j].second);

                temp_D.push_back(dist);
            }
            Distances.push_back(temp_D);
        }

        /* load rows and cols */
        vector<int> cols;
        vector<int> rows;

        /* find indices for cols */
        for (auto v: Distances) {
            auto temp = findMin(v);
            cols.push_back(temp);
        }

        /* rows calculation
           sort each mat row for rows calculation */
        vector<vector<float>> D_copy;
        for (auto v: Distances) {
            sort(v.begin(), v.end());
            D_copy.push_back(v);
        }

        /* use cols calc to find rows
           slice first elem of each column */
        vector<pair<float, int>> temp_rows;
        int k = 0;
        for (auto i: D_copy) {
            temp_rows.push_back(make_pair(i[0], k));
            k++;
        }
        
        for (auto const &x : temp_rows) {
            rows.push_back(x.second);
        }

        set<int> usedRows;
        set<int> usedCols;
        /* loop over the combination of the (rows, columns) index tuples */
        for (int i = 0; i < rows.size(); i++) {

            /* if we have already examined either the row or column value before, ignore it */
            if (usedRows.count(rows[i]) || usedCols.count(cols[i])) continue;
            /* Added maxDist logic here */
            if (Distances[rows[i]][cols[i]] > this->maxDist) continue; 

            /* otherwise, grab the object ID for the current row, set its new centroid,
               and reset the disappeared counter    */
            int objectID = objectIDs[rows[i]];
            for (int t = 0; t < this->objects.size(); t++) {
                if (this->objects[t].first == objectID) {
                    this->objects[t].second.first = inputCentroids[cols[i]].first;
                    this->objects[t].second.second = inputCentroids[cols[i]].second;
                }
            }
            this->disappeared[objectID] = 0;
            usedRows.insert(rows[i]);
            usedCols.insert(cols[i]);
        }

        /* compute indexes we have NOT examined yet */
        set<int> objRows;
        set<int> inpCols;

        for (int i = 0; i < objectCentroids.size(); i++) {
            objRows.insert(i);
        }

        for (int i = 0; i < inputCentroids.size(); i++) {
            inpCols.insert(i);
        }

        set<int> unusedRows;
        set<int> unusedCols;
        set_difference(objRows.begin(), objRows.end(), usedRows.begin(), usedRows.end(),
                       inserter(unusedRows, unusedRows.begin()));
        set_difference(inpCols.begin(), inpCols.end(), usedCols.begin(), usedCols.end(),
                       inserter(unusedCols, unusedCols.begin()));

        /* If objCentroids > InpCentroids, we need to check and see if some of these objects have potentially disappeared */
        if (objectCentroids.size() >= inputCentroids.size()) {
            /* loop over unused row indexes */
            for (auto row: unusedRows) {
                int objectID = objectIDs[row];
                this->disappeared[objectID] += 1;

                if (this->disappeared[objectID] > this->maxDisappear) {
                    this->objects.erase(remove_if(this->objects.begin(), this->objects.end(), [objectID](auto &elem) {
                        return elem.first == objectID;
                    }), this->objects.end());

                    this->path_keeper.erase(objectID);
                    this->disappeared.erase(objectID);
                }
            }
        } else {
            for (auto col: unusedCols) {
                this->register_Object(inputCentroids[col].first, inputCentroids[col].second);
            }
        }
    }
    /* loading path tracking points */
    if (!objects.empty()) {
        for (auto obj: objects) {

            if (path_keeper[obj.first].size() > 30) {
                path_keeper[obj.first].erase(path_keeper[obj.first].begin());
            }
            path_keeper[obj.first].push_back(make_pair(obj.second.first, obj.second.second));
        }
    }
    return this->objects;
}
