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
/*
/// <reference types="node" />

declare const NumUnistrokes: number
declare const NumPoints: number
declare const SquareSize: number
declare const Origin: number
declare const Diagonal: number
declare const HalfDiagonal: number
declare const AngleRange: number
declare const AnglePrecision: number
declare const Phi: number

export interface Point {
    X: number
    Y: number
}

export interface Rectangle {
    X: number
    Y: number
    Width: number
    Height: number
}

export class Unistroke {
    Name: string;
    Points: number[]
    Vector: number[]

    constructor(name: string, points: number[])
}

export class Result {
    Name: string
    Score: number
    Time: number
}

export class DollarRecognizer {
    Unistrokes: Unistroke[]

    constructor()

    Recognize(points: Point[], useProtractors: boolean): Result
    AddGesture(name: string, points: Point[]): number
    DeleteUserGesture(): number
}

export function Resample(points: Point[], n: number): Point[]
export function IndicativeAngle(points: point[]): number
export function RotateBy(points: Point[], radians: number): Point[]
export function ScaleTo(points: Point[], size: number): Point[]
export function TranslateTo(points: Point[], pt: Point): Points[]
export function Vectorize(points: Point[]): number[]
export function OptimalCosineDistance(v1: number[], v2: number[]): number
export function DistanceAtBestAngle(points: Point[], T: Object, a: number, b: number, threshold: number): number
export function Centroid(points: Point[]): Point
export function BoindingBox(points: Point[]): Rectangle
export function PathDistance(pts1: Points[], pts2: Points[]): number
export function PathLength(points: Point[]): number
export function Distance(p1: Point, p2: Point): number
export function Deg2Red(d: number): number;
*/