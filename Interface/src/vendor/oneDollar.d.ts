/// <reference types="node" />
//
// Point class
//
interface Point {
    X: number;
    Y: number;
}

//
// Rectangle class
//
interface Rectangle extends Point {
    Width: number;
    Height: number;
}

//
// Unistroke class: a unistroke template
//
declare class Unistroke {
    Name: string;
    Points: Point[];
    Vector: number[];

    constructor(name: string, points: Point[]);
}

//
// Result class
//
interface Result {
    Name: string;
    Score: number;
    Time: number;
}

//
// DollarRecognizer class
//
declare export class DollarRecognizer {
    Unistrokes: Unistroke[];

    constructor();

    Recognize(points: Point[]): Result;
}

declare const NumUnistrokes: number;
declare const NumPoints: number;
declare const SquareSize: number;
declare const Origin: Point;
declare const Diagonal: number;
declare const HalfDiagonal: number;
declare const AngleRange: number;
declare const AnglePrecision: number;
declare const Phi: number;

export function Centroid(points: Point[]): Point
export function BoundingBox(points: Point[]) : Rectangle