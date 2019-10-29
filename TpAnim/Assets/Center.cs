using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Center : MonoBehaviour
{
    public GameObject P1, P2, P3, target;
    public List<GameObject> points;
    LineRenderer lineRenderer;
    float reachable;
    List<float> distances;

    // Start is called before the first frame update
    void Start()
    {
        distances = new List<float>();
        reachable = 0.0f;

        for (int i = 0; i < points.Count - 1; i++)
        {
            float d = Vector3.Distance(points[i].transform.position, points[i + 1].transform.position);
            reachable += d;
            distances.Add(d);
        }

        lineRenderer = GetComponent<LineRenderer>();
        lineRenderer.positionCount = points.Count;
        lineRenderer.SetPosition(0, gameObject.transform.position);

        for(int i = 1; i < points.Count; i++)
            lineRenderer.SetPosition(i, points[i].transform.position);
    }

    // Update is called once per frame
    void Update()
    {
        FabrikAlgorithm();
        lineRenderer.positionCount = points.Count;
        lineRenderer.SetPosition(0, gameObject.transform.position);
        for (int i = 1; i < points.Count; i++)
            lineRenderer.SetPosition(i, points[i].transform.position);
    }

    void FabrikAlgorithm()
    {
        float dist = Vector3.Distance(P1.transform.position, target.transform.position);
        if(dist > reachable)
        {
            for(int i = 1; i < points.Count - 1; i++)
            {
                float r = Vector3.Distance(target.transform.position, points[i].transform.position);
                float lambda = Vector3.Distance(points[i + 1].transform.position, points[i].transform.position) / r;
                points[i + 1].transform.position = (1 - lambda) * points[i].transform.position + (lambda * target.transform.position);
            }
        }
        else
        {
           // Vector3 b = points[1].transform.position;
            for(int i = points.Count - 2; i >= 0; i--)
            {
                Vector3 nJoint = points[i + 1].transform.position + (target.transform.position - points[i + 1].transform.position).normalized * distances[i];
                float r = Vector3.Distance(points[i].transform.position, nJoint);
                float lambda = distances[i] / r;
                points[i].transform.position = (1.0f - lambda) * points[i + 1].transform.position + lambda * points[i].transform.position;
            }

            //points[1].transform.position = b;
            for (int i = 1; i < points.Count - 1; i++)
            {
                Vector3 nJoint = points[i].transform.position + (target.transform.position - points[i].transform.position).normalized * distances[i];
                float r = Vector3.Distance(points[i + 1].transform.position, nJoint);
                float lambda = distances[i] / r;
                points[i + 1].transform.position = (1.0f - lambda) * points[i].transform.position + lambda * points[i + 1].transform.position;
            }
        }
    }

    void anim()
    {
        Vector3 nP1, nP2, nP3;
        nP3 = target.transform.position;
        nP2 = nP3 + (P2.transform.position - nP3).normalized * Vector3.Distance(P2.transform.position, P3.transform.position);
        nP1 = nP2 + (P1.transform.position - nP2).normalized * Vector3.Distance(P1.transform.position, P2.transform.position);

        Vector3 aP1, aP2, aP3;
        aP1 = gameObject.transform.position + (nP1 - gameObject.transform.position).normalized * Vector3.Distance(gameObject.transform.position, P1.transform.position);
        aP2 = aP1 + (nP2 - aP1).normalized * Vector3.Distance(nP1, nP2);
        aP3 = aP2 + (nP3 - aP2).normalized * Vector3.Distance(nP2, nP3);

        P1.transform.position = aP1;
        P2.transform.position = aP2;
        P3.transform.position = aP3;
    }
}
